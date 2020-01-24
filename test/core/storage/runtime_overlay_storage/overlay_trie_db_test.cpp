/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include <memory>

#include "mock/core/storage/trie/trie_db_mock.hpp"
#include "storage/in_memory/in_memory_storage.hpp"
#include "storage/runtime_overlay_storage/impl/overlay_trie_db_impl.hpp"

using kagome::storage::InMemoryStorage;
using kagome::storage::runtime_overlay_storage::OverlayTrieDb;
using kagome::storage::trie::TrieDbMock;

TEST(OverlayTrieDbTest, Create) {
  auto trie = std::make_shared<TrieDbMock>();

  auto overlay = OverlayTrieDb(
      std::make_unique<InMemoryStorage<OverlayTrieDb::StorageKey,
                                       OverlayTrieDb::ChangesStorageValue>>(),
      trie);
}
