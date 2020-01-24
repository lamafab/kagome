/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "storage/runtime_overlay_storage/impl/overlay_trie_db_impl.hpp"

#include <boost/optional.hpp>

#include "scale/scale.hpp"

namespace kagome::storage::runtime_overlay_storage {

  const OverlayTrieDbImpl::StorageKey OverlayTrieDbImpl::kCurrentExtrinsicIdKey {":extrinsic_index"};


  OverlayTrieDbImpl::OverlayTrieDbImpl(std::unique_ptr<ChangesStorage> changes_storage,
                               std::shared_ptr<trie::TrieDb> true_storage)
      : changes_storage_{std::move(changes_storage)},
        true_storage_{std::move(true_storage)} {
    BOOST_ASSERT(changes_storage_ != nullptr);
    BOOST_ASSERT(true_storage_ != nullptr);
  }

  std::unique_ptr<face::MapCursor<common::Buffer, common::Buffer>>
  OverlayTrieDbImpl::cursor() {
    return true_storage_->cursor();
  }

  std::unique_ptr<face::MapCursor<OverlayTrieDbImpl::StorageKey, OverlayTrieDbImpl::ChangesStorageValue>>
  OverlayTrieDbImpl::changesCursor() {
    return changes_storage_->cursor();
  }

  outcome::result<kagome::common::Buffer> OverlayTrieDbImpl::get(
      const kagome::common::Buffer &key) const {
    if (changes_storage_->contains(key)) {
      OUTCOME_TRY(val, changes_storage_->get(key));
      return val.value;
    }
    return true_storage_->get(key);
  }

  bool OverlayTrieDbImpl::contains(const kagome::common::Buffer &key) const {
    if (changes_storage_->contains(key)) {
      return true;
    }
    return true_storage_->contains(key);
  }

  outcome::result<void> OverlayTrieDbImpl::put(
      const kagome::common::Buffer &key, const kagome::common::Buffer &value) {
    Buffer buf {value};
    return put(key, std::move(buf));
  }

  outcome::result<void> OverlayTrieDbImpl::put(const kagome::common::Buffer &key,
                                           kagome::common::Buffer &&value) {
    OUTCOME_TRY(extrinsic_id_buf, true_storage_->get(kCurrentExtrinsicIdKey));
    OUTCOME_TRY(extrinsic_id, scale::decode<ExtrinsicId>(extrinsic_id_buf));

    if (changes_storage_->contains(key)) {
      OUTCOME_TRY(v, changes_storage_->get(key));
      v.is_dirty = true;
      v.value = value;
      v.changes_extrinsics.push_back(extrinsic_id);

    } else {
      ChangesStorageValue v{.value = value,
          .changes_extrinsics = ExtrinsicSet {extrinsic_id},
          .is_dirty = true};
      OUTCOME_TRY(changes_storage_->put(key, std::move(v)));

    }
  }

  outcome::result<void> OverlayTrieDbImpl::remove(
      const kagome::common::Buffer &key) {
    OUTCOME_TRY(changes_storage_->remove(key));
    return true_storage_->remove(key);
  }

  kagome::common::Buffer OverlayTrieDbImpl::getRootHash() const {
    return true_storage_->getRootHash();
  }

  outcome::result<void> OverlayTrieDbImpl::clearPrefix(
      const common::Buffer &buf) {
    std::list<StorageKey> to_delete {};
    for(auto&& c = changes_storage_->cursor(); c->isValid(); c->next()) {
      if(c->key().subbuffer(0, buf.size()) == buf) {
        to_delete.push_back(c->key());
      }
    }
    for(auto&& k: to_delete) {
      OUTCOME_TRY(changes_storage_->remove(k));
    }
    return true_storage_->clearPrefix(buf);
  }

  bool OverlayTrieDbImpl::empty() const {
    return changes_storage_->empty() and true_storage_->empty();
  }

  std::unique_ptr<kagome::storage::face::WriteBatch<kagome::storage::Buffer,
                                                    kagome::storage::Buffer>>
  OverlayTrieDbImpl::batch() {
    return std::unique_ptr<face::WriteBatch<Buffer, Buffer>>();
  }

}  // namespace kagome::storage::runtime_overlay_storage
