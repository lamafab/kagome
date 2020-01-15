/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP
#define KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP

#include "outcome/outcome.hpp"
#include "common/blob.hpp"

namespace kagome::blockchain {

  class OverlayedChanges;

  class ChangesTrie {
   public:
    static outcome::result<ChangesTrie> create(
        const OverlayedChanges &changes,
        backend,
        storage,
        const common::Hash256 &parent_hash);

   private:
  };

}

#endif  // KAGOME_BLOCKCHAIN_CHANGES_TRIE_HPP
