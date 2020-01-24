/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_IMPL_HPP
#define KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_IMPL_HPP

#include "storage/runtime_overlay_storage/overlay_trie_db.hpp"
#include "common/buffer.hpp"

namespace kagome::storage::runtime_overlay_storage {

  /**
   * Wrapper to TrieDb used to grasp and contain changes to the runtime storage
   * for easier construction of the changes trie and also serving as an cache
   * for the true runtime storage
   */
  class OverlayTrieDbImpl : public OverlayTrieDb {
   public:
    using ChangesStorage = face::GenericMap<StorageKey, ChangesStorageValue>;

   public:
    static const StorageKey kCurrentExtrinsicIdKey;

    OverlayTrieDbImpl(std::unique_ptr<ChangesStorage> changes_storage,
                  std::shared_ptr<trie::TrieDb> true_storage);

    ~OverlayTrieDbImpl() override = default;

    std::unique_ptr<face::WriteBatch<Buffer, Buffer>> batch() override;

    /**
     * Cursor over the true storage
     */
    std::unique_ptr<face::MapCursor<common::Buffer, common::Buffer>> cursor()
        override;

    /**
     * Cursor over the changes storage
     */
    std::unique_ptr<face::MapCursor<StorageKey, ChangesStorageValue>>
    changesCursor();

    /**
     * The root hash of the true storage
     */
    Buffer getRootHash() const override;

    /**
     * Removes the entries of the given prefix from both the changes storage and
     * the storage
     */
    outcome::result<void> clearPrefix(const common::Buffer &buf) override;

    bool empty() const override;

    outcome::result<common::Buffer> get(
        const common::Buffer &key) const override;

    bool contains(const common::Buffer &key) const override;

    outcome::result<void> put(const common::Buffer &key,
                              const common::Buffer &value) override;

    outcome::result<void> put(const common::Buffer &key,
                              common::Buffer &&value) override;

    outcome::result<void> remove(const common::Buffer &key) override;

   private:
    std::unique_ptr<ChangesStorage> changes_storage_;
    std::shared_ptr<trie::TrieDb> true_storage_;
  };

}  // namespace kagome::storage::runtime_overlay_storage

#endif  // KAGOME_STORAGE_RUNTIME_OVERLAY_STORAGE_OVERLAY_TRIE_DB_IMPL_HPP
