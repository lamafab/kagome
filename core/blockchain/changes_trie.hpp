/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP
#define KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP

#include "common/blob.hpp"
#include "common/buffer.hpp"
#include "outcome/outcome.hpp"
#include "primitives/common.hpp"
#include "storage/face/generic_map.hpp"
#include "storage/runtime_overlay_storage/impl/overlay_trie_db_impl.hpp"
#include "storage/trie/impl/calculate_tree_root.hpp"

namespace kagome::blockchain {

  class ChangesTrie {
   public:
    struct ExtrinsicIndex {
      primitives::BlockNumber number;
      common::Buffer key;
    };

    static outcome::result<std::unique_ptr<ChangesTrie>> create(
        const std::shared_ptr<storage::runtime_overlay_storage::OverlayTrieDb>
            &overlay_storage,
        std::unique_ptr<
            storage::face::PersistentMap<common::Buffer, common::Buffer>>
            content_storage,
        const primitives::BlockNumber &parent_number) {
      auto trie = ChangesTrie();
      trie.content_storage_ = std::move(content_storage);

      for (auto it = overlay_storage->changesCursor(); it->isValid();
           it->next()) {
        auto &&key = it->key();
        auto &&value = it->value();
        if (value.changes_extrinsics.empty()) {
          continue;
        }
        // TODO(Harrm) ignore temporary values (values that have null value at
        // the end of operation AND are not in storage at the beginning of
        // operation

        ExtrinsicIndex idx{.key = key, .number = parent_number + 1};

        OUTCOME_TRY(extrs, scale::encode(value.changes_extrinsics));
        OUTCOME_TRY(
            trie.content_storage_->put(it->key(), common::Buffer{extrs}));
      }
      return std::make_unique<ChangesTrie>(std::move(trie));
    }

    outcome::result<common::Hash256> getRoot() const {
      std::vector<std::pair<common::Buffer, common::Buffer>> pairs;
      for (auto it = content_storage_->cursor(); it->isValid(); it->next()) {
        pairs.emplace_back(std::make_pair(it->key(), it->value()));
      }
      OUTCOME_TRY(
          root,
          storage::trie::calculateTrieRoot<common::Buffer, common::Buffer>(
              pairs.begin(), pairs.end()));
      common::Hash256 hash;
      std::move(root.begin(), root.end(), hash.begin());
      return hash;
    }

    const storage::face::PersistentMap<common::Buffer, common::Buffer>
        &getContent() const {
      return *content_storage_;
    }

   private:
    std::unique_ptr<
        storage::face::PersistentMap<common::Buffer, common::Buffer>>
        content_storage_;
  };

}  // namespace kagome::blockchain

#endif  // KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP
