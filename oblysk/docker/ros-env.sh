# Source ROS 2, the Open-RMF workspace, and (when built) the oblysk overlay.
# No `set -u`: the ROS setup scripts reference unbound variables.
source /opt/ros/jazzy/setup.bash
source /ws/install/setup.bash
if [ -f /overlay/install/setup.bash ]; then
  source /overlay/install/setup.bash
fi
