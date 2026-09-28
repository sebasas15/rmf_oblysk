#include <memory>

#include <rclcpp/rclcpp.hpp>
#include <rmf_task_msgs/msg/api_request.hpp>

#include "oblysk_dispatch/accept.hpp"

namespace {

using rmf_task_msgs::msg::ApiRequest;

// Same QoS as the stock task API clients (rmf_demos_tasks), recorded in oblysk/docker/README.md.
rclcpp::QoS task_api_qos() { return rclcpp::QoS(rclcpp::KeepLast(10)).reliable().transient_local(); }

class DispatchNode : public rclcpp::Node {
 public:
  DispatchNode() : rclcpp::Node("oblysk_dispatch") {
    pub_ = create_publisher<ApiRequest>("/task_api_requests", task_api_qos());
    sub_ = create_subscription<ApiRequest>(
        "/oblysk/validated_task_requests", task_api_qos(),
        [this](const ApiRequest::ConstSharedPtr msg) { on_request(*msg); });
  }

 private:
  void on_request(const ApiRequest& msg) {
    const auto fwd = oblysk::dispatch::accept(msg.request_id, msg.json_msg);
    if (!fwd) {
      // L0 promises nothing about malformed input; a named refusal is INV-RMF-5, which is L1.
      RCLCPP_WARN(get_logger(), "dropped request_id=%s: not a ValidatedTaskRequest",
                  msg.request_id.c_str());
      return;
    }
    ApiRequest out;
    out.request_id = fwd->request_id;
    out.json_msg = fwd->json_msg;
    pub_->publish(out);
    RCLCPP_INFO(get_logger(), "forwarded request_id=%s", out.request_id.c_str());
  }

  rclcpp::Publisher<ApiRequest>::SharedPtr pub_;
  rclcpp::Subscription<ApiRequest>::SharedPtr sub_;
};

}  // namespace

int main(int argc, char** argv) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<DispatchNode>());
  rclcpp::shutdown();
  return 0;
}
