# syntax=docker/dockerfile:1
# Stage 0 RMF + Gazebo image for the oblysk overlay. Build context: the rmf_oblysk repo root.
# Carries forward the prototype's proven recipe (oblysk_rms_sim docker/Dockerfile.rmf): Open-RMF
# on ROS 2 Jazzy built from FULL SOURCE — the Jazzy binary feed lacks the reservation packages the
# demos need — pinned to exact commits in oblysk/docker/rmf.repos, on a digest-pinned base.
FROM ros:jazzy@sha256:6513503d0b10e919fbe8134981d4f9d19b5c1f9b045b87a9fe3b0b9e03e7c2a9 AS base
ENV DEBIAN_FRONTEND=noninteractive
SHELL ["/bin/bash", "-c"]

# 1) Build toolchain + CycloneDDS. RMF and Gazebo deps are resolved by rosdep below.
COPY oblysk/docker/rmf-packages.txt /tmp/rmf-packages.txt
RUN apt-get update && \
    grep -vE '^#|^[[:space:]]*$' /tmp/rmf-packages.txt | xargs apt-get install -y --no-install-recommends && \
    rm -rf /var/lib/apt/lists/*

# 2) Import the pinned, version-matched Open-RMF source set.
WORKDIR /ws
COPY oblysk/docker/rmf.repos /ws/rmf.repos
RUN mkdir -p src && vcs import src < rmf.repos && echo "imported $(ls src | wc -l) repos"

# 3) Resolve build/runtime deps (pulls Gazebo Harmonic).
RUN apt-get update && \
    (rosdep init 2>/dev/null || true) && rosdep update && \
    rosdep install --from-paths src --ignore-src -r -y --rosdistro jazzy && \
    rm -rf /var/lib/apt/lists/*

# 4) Build up to the demos. Parallelism capped: the dev host OOMs near 5.8 GB.
# NO_DOWNLOAD_MODELS skips the buggy in-build Fuel download; step 5 fetches instead.
ENV MAKEFLAGS="-j2"
RUN source /opt/ros/jazzy/setup.bash && \
    colcon build --packages-up-to rmf_demos_gz rmf_demos_maps \
      --parallel-workers 1 \
      --cmake-args -DCMAKE_BUILD_TYPE=Release -DNO_DOWNLOAD_MODELS=ON

# 5) Bake the office world's Gazebo models in, so runtime needs no network.
RUN source /opt/ros/jazzy/setup.bash && source /ws/install/setup.bash && \
    BY=$(find /ws/src -name office.building.yaml | head -1) && \
    ros2 run rmf_building_map_tools building_map_model_downloader "$BY" -e /root/.gazebo/models && \
    echo "office models baked: $(ls /root/.gazebo/models | wc -l)"

COPY oblysk/docker/ros-env.sh /opt/oblysk/ros-env.sh
ENV RMW_IMPLEMENTATION=rmw_cyclonedds_cpp \
    ROS_DOMAIN_ID=42 \
    LIBGL_ALWAYS_SOFTWARE=1 \
    QT_QPA_PLATFORM=offscreen
CMD ["bash"]

FROM base AS overlay
# The oblysk overlay: our packages built against the RMF workspace.
COPY oblysk/src /overlay/src
RUN apt-get update && source /opt/ros/jazzy/setup.bash && \
    rosdep install --from-paths /overlay/src --ignore-src -r -y --rosdistro jazzy && \
    rm -rf /var/lib/apt/lists/*
WORKDIR /overlay
RUN source /opt/ros/jazzy/setup.bash && source /ws/install/setup.bash && \
    colcon build --cmake-args -DCMAKE_BUILD_TYPE=Release
