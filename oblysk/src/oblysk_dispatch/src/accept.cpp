#include "oblysk_dispatch/accept.hpp"

#include <nlohmann/json.hpp>

namespace oblysk::dispatch {

std::optional<Forward> accept(std::string_view request_id, std::string_view json_msg) {
  const auto body = nlohmann::json::parse(json_msg, nullptr, /*allow_exceptions=*/false);
  if (body.is_discarded() || !body.is_object()) {
    return std::nullopt;
  }
  const auto rmf = body.find("rmf");
  if (rmf == body.end() || !rmf->is_object()) {
    return std::nullopt;
  }
  return Forward{std::string(request_id), rmf->dump()};
}

}  // namespace oblysk::dispatch
