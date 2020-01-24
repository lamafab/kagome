
#include "blockchain/changes_trie.hpp"

#include <gtest/gtest.h>

#include "mock/core/storage/runtime_overlay_storage/overlay_trie_db_mock.hpp"
#include "scale/scale.hpp"
#include "storage/in_memory/in_memory_storage.hpp"
#include "storage/runtime_overlay_storage/impl/overlay_trie_db_impl.hpp"
#include "testutil/literals.hpp"
#include "testutil/outcome.hpp"

using kagome::storage::runtime_overlay_storage::OverlayTrieDbImpl;
using kagome::storage::runtime_overlay_storage::OverlayTrieDbMock;
using ChangesStorageValue = kagome::storage::runtime_overlay_storage::
    OverlayTrieDb::ChangesStorageValue;
using kagome::blockchain::ChangesTrie;
using kagome::storage::InMemoryStorage;
using ChangesStorageKey =
    kagome::storage::runtime_overlay_storage::OverlayTrieDb::StorageKey;
using kagome::common::Buffer;
using kagome::common::Hash256;
using testing::Invoke;
using testing::Return;

template <typename K, typename V>
class OwningCursor : public InMemoryStorage<K, V>::InMemoryStorageCursor {
 public:
  explicit OwningCursor(std::shared_ptr<std::map<K, V>> storage)
      : InMemoryStorage<K, V>::InMemoryStorageCursor{*storage},
        storage_{std::move(storage)} {}
  ~OwningCursor() override = default;

  using InMemoryStorage<K, V>::InMemoryStorageCursor::isValid;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::key;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::seekToLast;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::value;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::next;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::prev;
  using InMemoryStorage<K, V>::InMemoryStorageCursor::seekToFirst;

 private:
  std::shared_ptr<std::map<K, V>> storage_;
};

std::shared_ptr<OverlayTrieDbMock> prepare_overlay() {
  auto overlay_storage =
      std::make_shared<std::map<ChangesStorageKey, ChangesStorageValue>>();
  overlay_storage->insert(std::make_pair(
      OverlayTrieDbImpl::kCurrentExtrinsicIdKey,
      ChangesStorageValue{.value = Buffer{kagome::scale::encode(3).value()},
                          .changes_extrinsics = std::vector<uint32_t>{1}}));
  overlay_storage->insert(std::make_pair(
      Buffer{1},
      ChangesStorageValue{.value = Buffer{100},
                          .changes_extrinsics = std::vector<uint32_t>{1}}));
  auto overlay = std::make_shared<OverlayTrieDbMock>();
  EXPECT_CALL(*overlay, changesCursor()).WillOnce(Invoke([overlay_storage]() {
    auto cursor =
        std::make_unique<OwningCursor<ChangesStorageKey, ChangesStorageValue>>(
            overlay_storage);
    return cursor;
  }));
  return overlay;
}

TEST(ChangesTrieTest, Create) {
  auto overlay = prepare_overlay();
  auto storage = std::make_unique<InMemoryStorage<Buffer, Buffer>>();
  EXPECT_OUTCOME_TRUE(
      changes_trie,
      ChangesTrie::create(overlay, std::move(storage), Hash256{}));
  ASSERT_EQ(
      changes_trie->getRoot().value(),
      "bb0c2ef6e1d36d5490f9766cfcc7dfe2a6ca804504c3bb206053890d6dd02376"_hash256);
}
