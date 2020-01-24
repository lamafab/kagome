/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_STORAGE_IN_MEMORY_IN_MEMORY_STORAGE_HPP
#define KAGOME_STORAGE_IN_MEMORY_IN_MEMORY_STORAGE_HPP

#include <memory>

#include "outcome/outcome.hpp"
#include "storage/face/persistent_map.hpp"
#include "storage/in_memory/in_memory_batch.hpp"

namespace kagome::storage {

  /**
   * Simple storage that conforms PersistentMap interface
   * Mostly needed to have an in-memory trie in tests to avoid integration with
   * LevelDB
   */
  template <typename K, typename V>
  class InMemoryStorage : public face::PersistentMap<K, V> {
   public:
    class InMemoryStorageCursor;

    ~InMemoryStorage() override = default;

    outcome::result<V> get(const K &key) const {
      if (storage_.find(key) != storage_.end()) {
        return storage_.at(key);
      }
      return V{};
    }

    outcome::result<void> put(const K &key, const V &value) {
      storage_[key] = value;
      return outcome::success();
    }

    outcome::result<void> put(const K &key, V &&value) {
      storage_[key] = std::move(value);
      return outcome::success();
    }

    bool contains(const K &key) const {
      return storage_.find(key) != storage_.end();
    }

    outcome::result<void> remove(const K &key) {
      storage_.erase(key);
      return outcome::success();
    }

    std::unique_ptr<kagome::storage::face::WriteBatch<K, V>> batch() {
      return std::make_unique<InMemoryBatch<InMemoryStorage, K, V>>(*this);
    }

    std::unique_ptr<kagome::storage::face::MapCursor<K, V>> cursor() {
      return std::make_unique<InMemoryStorageCursor>(storage_);
    }

    bool empty() const override {
      return storage_.empty();
    }

    class InMemoryStorageCursor : public face::MapCursor<K, V> {
     public:
      explicit InMemoryStorageCursor(std::map<K, V> &map)
          : map_{map}, it_{map.begin()} {}

      void seekToFirst() override {
        it_ = map_.begin();
      }
      void seek(const K &key) override {
        it_ = map_.find(key);
      }
      void seekToLast() override {
        it_ = std::prev(map_.end());
      }
      bool isValid() const override {
        if(it_ != map_.end()) {
          return true;
        }
        return false;
      }
      void next() override {
        it_ = std::next(it_);
      }
      void prev() override {
        it_ = std::prev(it_);
      }
      K key() const override {
        return it_->first;
      }
      V value() const override {
        return it_->second;
      }

     private:
      std::map<K, V> &map_;
      typename std::map<K, V>::iterator it_;
    };

   private:
    std::map<K, V> storage_;
  };

}  // namespace kagome::storage

#endif  // KAGOME_STORAGE_IN_MEMORY_IN_MEMORY_STORAGE_HPP
