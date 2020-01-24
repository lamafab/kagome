# Copyright Soramitsu Co., Ltd. All Rights Reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(overlay_trie_db impl/overlay_trie_db_impl.cpp)
target_link_libraries(overlay_trie_db
    buffer
    Boost::boost
    scale
    )
