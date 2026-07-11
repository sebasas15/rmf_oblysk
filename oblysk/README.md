# rmf_oblysk

**Responsibility:** Mirror (fork) of `open-rmf/rmf` — the Open-RMF control core
(`rmf_task`, `rmf_traffic`, API server, `rmf-web`). We run and patch demos/configs;
upstream tracked via the `upstream` git remote.

**Toolchain:** ROS 2 Jazzy · colcon · vcstool. Devcontainer on `ros:jazzy`.

**Architecture:** see `architecture_v3` — `@ref control`, `@ref rmf_eval`.

## Sync with upstream

    git remote add upstream https://github.com/open-rmf/rmf.git   # once
    git fetch upstream && git merge upstream/main

Status: fork overlay only — vendor source unchanged.
