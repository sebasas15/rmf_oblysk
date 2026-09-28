#!/usr/bin/env bash
# rmf service entrypoint: launch the Stage 0 bring-up, then unpause Gazebo.
set -o pipefail
source /opt/oblysk/ros-env.sh

ros2 launch /opt/oblysk/launch/stage0_rmf.launch.xml &
LAUNCH=$!
trap 'kill -INT "$LAUNCH" 2>/dev/null; wait "$LAUNCH"' TERM INT

# Gazebo starts paused, which freezes /clock (prototype 00-base-rmf-demos/rmf-smoke.sh).
for attempt in $(seq 1 30); do
  sleep 4
  if gz service -s /world/sim_world/control \
       --reqtype gz.msgs.WorldControl --reptype gz.msgs.Boolean \
       --timeout 5000 --req 'pause: false' 2>/dev/null | grep -q 'data: true'; then
    echo "stage0: gazebo unpaused (attempt $attempt)"
    break
  fi
done

wait "$LAUNCH"
