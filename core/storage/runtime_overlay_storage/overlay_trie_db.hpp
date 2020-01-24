/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_HPP
#define KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_HPP

#include "common/buffer.hpp"
#include "storage/trie/trie_db.hpp"

namespace kagome::storage::runtime_overlay_storage {

  /**
   * Wrapper to TrieDb used to grasp and contain changes to the runtime storage
   * for easier construction of the changes trie and also serving as an cache
   * for the true runtime storage
   */
  class OverlayTrieDb : public trie::TrieDb {
   public:
    using ExtrinsicId = uint32_t;
    using StorageKey = common::Buffer;
    using StorageValue = common::Buffer;
    using ExtrinsicSet = std::vector<ExtrinsicId>;
    struct ChangesStorageValue {
      // the new value
      StorageValue value;
      // extrinsics that changed the value
      ExtrinsicSet changes_extrinsics{};
      // whether the value has been written out to the true storage
      bool is_dirty{true};
    };

   public:
    virtual ~OverlayTrieDb() override = default;

    virtual std::unique_ptr<face::WriteBatch<Buffer, Buffer>> batch() = 0;

    /**
     * Cursor over the true storage
     */
    virtual std::unique_ptr<face::MapCursor<common::Buffer, common::Buffer>> cursor() = 0;

    /**
     * Cursor over the changes storage
     */
    virtual std::unique_ptr<face::MapCursor<StorageKey, ChangesStorageValue>>
    changesCursor() = 0;

    /**
     * The root hash of the true storage
     */
    virtual Buffer getRootHash() const = 0;

    /**
     * Removes the entries of the given prefix from both the changes storage and
     * the storage
     */
    virtual outcome::result<void> clearPrefix(const common::Buffer &buf) = 0;

    virtual bool empty() const = 0;

    virtual outcome::result<common::Buffer> get(
        const common::Buffer &key) const = 0;

    virtual bool contains(const common::Buffer &key) const = 0;

    virtual outcome::result<void> put(const common::Buffer &key,
                              const common::Buffer &value) = 0;

    virtual outcome::result<void> put(const common::Buffer &key,
                              common::Buffer &&value) = 0;

    virtual outcome::result<void> remove(const common::Buffer &key) = 0;
  };

}  // namespace kagome::storage::runtime_overlay_storage

#endif  // KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_HPP
