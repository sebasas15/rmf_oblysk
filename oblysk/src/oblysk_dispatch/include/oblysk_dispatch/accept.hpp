#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace oblysk::dispatch {

// What control.dispatch hands to rmf_task: the request id and the `rmf` body of a
// ValidatedTaskRequest (design/pages/04-control-rmf-core.md §3.1) with the oblysk_envelope removed.
struct Forward {
  std::string request_id;
  std::string json_msg;
};

// The provided accept() operation of docs/design/control/dispatch.md §2, at L0: any
// well-formed request is forwarded unconditionally. L1 puts INV-RMF-1's eligibility filter and
// INV-RMF-5's declared refusal reasons here; nothing else in the node moves.
//
// Returns nullopt when json_msg is not a JSON object with an object-valued "rmf" member.
std::optional<Forward> accept(std::string_view request_id, std::string_view json_msg);

}  // namespace oblysk::dispatch
