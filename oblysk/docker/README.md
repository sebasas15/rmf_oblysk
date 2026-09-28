# oblysk RMF + Gazebo image (Stage 0)

`Dockerfile` targets:

- `base` — Open-RMF on ROS 2 Jazzy from full source (`rmf.repos`, commit-pinned), Gazebo
  Harmonic, office-world models baked in. Carried forward from the prototype's
  `docker/Dockerfile.rmf` (`oblysk_rms_sim`), which reached a green `rmf-smoke`.
- `overlay` — `base` plus the oblysk packages under `oblysk/src`, `oblysk/launch`, and the
  `rmf` service entrypoint.

Upstream's `Dockerfile` at the repo root is untouched; it targets another distro and is not
proven for this stack.

Build (from the repo root; first build takes hours):

    docker build -f oblysk/docker/Dockerfile --target base -t oblysk/rmf-base:stage0 .

## Known weak pins (inherited, stated rather than hidden)

- Fuel meshes (office models) are not version-pinned upstream.
- apt packages are not content-hash-pinned (the standard ROS-image limitation).

## Stock facts this overlay relies on (recorded 2026-09-24 from the built image)

- Task API QoS: `rmf_task_ros2` `Dispatcher.cpp` subscribes `task_api_requests` and publishes
  `task_api_responses` reliable, transient-local, keep-last depth `10`. (`rmf_demos_tasks/dispatch_patrol.py`
  publishes requests at depth `1`; depth is local history and does not affect QoS matching.)
- Office fleet config: `/ws/install/rmf_demos/share/rmf_demos/config/office/tinyRobot_config.yaml`.
- `rmf_demos_gz/simulation.launch.xml` reads `$(var headless)` without declaring it: every
  includer must pass `headless`.
- Office nav graph: `/ws/install/rmf_demos_maps/share/rmf_demos_maps/maps/office/nav_graphs/0.yaml`.
