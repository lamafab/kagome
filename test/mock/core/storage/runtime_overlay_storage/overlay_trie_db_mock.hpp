/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef KAGOME_STORAGE_OVERLAY_TRIE_DB_MOCK_HPP
#define KAGOME_STORAGE_OVERLAY_TRIE_DB_MOCK_HPP

#include <gmock/gmock.h>

#include "storage/runtime_overlay_storage/impl/overlay_trie_db_impl.hpp"

namespace kagome::storage::runtime_overlay_storage {

  class OverlayTrieDbMock : public OverlayTrieDb {
   public:
    MOCK_METHOD0(batch, std::unique_ptr<face::WriteBatch<Buffer, Buffer>>());

    MOCK_METHOD0(
        cursor,
        std::unique_ptr<face::MapCursor<common::Buffer, common::Buffer>>());

    MOCK_METHOD0(
        changesCursor,
        std::unique_ptr<face::MapCursor<StorageKey, ChangesStorageValue>>());

    MOCK_CONST_METHOD0(getRootHash, Buffer());

    MOCK_METHOD1(clearPrefix, outcome::result<void>(const common::Buffer &buf));

    MOCK_CONST_METHOD0(empty, bool());

    MOCK_CONST_METHOD1(
        get, outcome::result<common::Buffer>(const common::Buffer &key));

    MOCK_CONST_METHOD1(contains, bool(const common::Buffer &key));

    MOCK_METHOD2(put,
                 outcome::result<void>(const common::Buffer &key,
                                       const common::Buffer &value));

    outcome::result<void> put (const common::Buffer &key,
                          common::Buffer &&value) override {
      return rvalue_put(key, std::move(value));
    }

    MOCK_METHOD2(rvalue_put,
                 outcome::result<void>(const common::Buffer &key,
                                       common::Buffer value));

    MOCK_METHOD1(remove, outcome::result<void> (const common::Buffer &key));
  };

}  // namespace kagome::storage::runtime_overlay_storage

#endif  // KAGOME_OVERLAY_TRIE_DB_HPP
