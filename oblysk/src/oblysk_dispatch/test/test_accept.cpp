#include <gtest/gtest.h>

#include <nlohmann/json.hpp>

#include "oblysk_dispatch/accept.hpp"

using nlohmann::json;
using oblysk::dispatch::accept;

namespace {

const json kRmf = {
    {"type", "dispatch_task_request"},
    {"request",
     {{"category", "patrol"},
      {"description", {{"places", {"pantry", "lounge"}}, {"rounds", 1}}}}}};

std::string validated(const json& rmf) {
  return json{{"rmf", rmf}, {"oblysk_envelope", {{"ir_id", "ir-1"}}}}.dump();
}

}  // namespace

TEST(Accept, ForwardsRmfBodyJsonEqual) {
  const auto f = accept("r-1", validated(kRmf));
  ASSERT_TRUE(f.has_value());
  EXPECT_EQ(json::parse(f->json_msg), kRmf);
}

TEST(Accept, StripsEnvelope) {
  const auto f = accept("r-1", validated(kRmf));
  ASSERT_TRUE(f.has_value());
  EXPECT_FALSE(json::parse(f->json_msg).contains("oblysk_envelope"));
}

TEST(Accept, PreservesRequestId) {
  const auto f = accept("oblysk-stage0-abc", validated(kRmf));
  ASSERT_TRUE(f.has_value());
  EXPECT_EQ(f->request_id, "oblysk-stage0-abc");
}

TEST(Accept, DropsMalformedJson) { EXPECT_FALSE(accept("r-1", "{not json").has_value()); }

TEST(Accept, DropsMissingRmfBody) {
  EXPECT_FALSE(accept("r-1", json{{"oblysk_envelope", json::object()}}.dump()).has_value());
}

TEST(Accept, DropsNonObjectRmfBody) {
  EXPECT_FALSE(accept("r-1", json{{"rmf", "text"}}.dump()).has_value());
}

TEST(Accept, DropsNonObjectTopLevel) { EXPECT_FALSE(accept("r-1", "[1,2]").has_value()); }
