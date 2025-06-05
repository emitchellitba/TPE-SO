#!/bin/bash

MODE=${1:-USE_SIMPLE_MM}

if [[ "$MODE" != "USE_SIMPLE_MM" && "$MODE" != "USE_BUDDY_MM" ]]; then
    echo "Error: MODE must be USE_SIMPLE_MM or USE_BUDDY_MM"
    exit 1
fi

docker start TPE_SO
docker exec -it TPE_SO make clean -C /root/Toolchain
docker exec -it TPE_SO make clean -C /root/
docker exec -it TPE_SO make -C /root/Toolchain
docker exec -it TPE_SO make CFLAGS="-D${MODE}" -C /root/
docker exec -it TPE_SO make clean -C /root/Kernel
docker exec -it TPE_SO make clean -C /root/Userland
docker exec -it TPE_SO chmod 777 /root/Image/x64BareBonesImage.qcow2
