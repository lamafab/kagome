/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_IN_MEMORY_BATCH_HPP
#define KAGOME_IN_MEMORY_BATCH_HPP

#include <map>

#include "storage/face/write_batch.hpp"

namespace kagome::storage {

  template <typename Storage, typename K, typename V>
  class InMemoryBatch
      : public kagome::storage::face::WriteBatch<K,
                                                 V> {
   public:
    explicit InMemoryBatch(Storage &db) : db{db} {}

    outcome::result<void> put(const K &key,
                              const V &value) override {
      entries[key] = value;
      return outcome::success();
    }

    outcome::result<void> put(const K &key,
                              V &&value) override {
      entries[key] = std::move(value);
      return outcome::success();
    }

    outcome::result<void> remove(const K &key) override {
      entries.erase(key);
      return outcome::success();
    }

    outcome::result<void> commit() override {
      for (auto &entry : entries) {
        OUTCOME_TRY(
            db.put(entry.first, entry.second));
      }
      return outcome::success();
    }

    void clear() override {
      entries.clear();
    }

   private:
    std::map<K, V> entries;
    Storage &db;
  };
}  // namespace kagome::storage

#endif  // KAGOME_IN_MEMORY_BATCH_HPP
