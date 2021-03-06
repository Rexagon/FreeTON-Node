#pragma once
#include "td/utils/buffer.h"
#include "td/utils/common.h"
#include "td/utils/optional.h"

#include "td/actor/actor.h"

#include <map>

#include "LoadSpeed.h"

namespace ton {
using PeerId = td::uint64;
using PartId = td::uint32;

struct PeerState {
  struct State {
    bool will_upload{false};
    bool want_download{false};
    auto key() const {
      return std::tie(will_upload, want_download);
    }
    bool operator==(const State &other) const {
      return key() == other.key();
    }
  };
  State node_state_;
  td::optional<State> peer_state_;
  bool peer_online_{false};

  struct Part {
    td::BufferSlice proof;
    td::BufferSlice data;
  };
  std::map<PartId, td::optional<td::Result<Part>>> node_queries_;
  std::map<PartId, td::optional<td::Result<Part>>> peer_queries_;

  // Peer -> Node
  // update are added to this vector, so reader will be able to process all changes
  std::vector<PartId> peer_ready_parts_;

  // Node -> Peer
  // writer writes all new parts to this vector. This state will be eventually synchornized with a peer
  std::vector<PartId> node_ready_parts_;

  td::actor::ActorId<> node;
  td::actor::ActorId<> peer;

  LoadSpeed upload;
  LoadSpeed download;

  void notify_node();
  void notify_peer();
};
}  // namespace ton
