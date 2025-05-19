#!/bin/bash

MODE=${1:-USE_SIMPLE_MM} 

docker start TPE_SO
docker exec -it TPE_SO make clean -C /root/Toolchain
docker exec -it TPE_SO make clean -C /root/
docker exec -it TPE_SO make -C /root/Toolchain
docker exec -it TPE_SO make CFLAGS="-D${MODE}" -C /root/
docker exec -it TPE_SO make clean -C /root/Kernel
docker exec -it TPE_SO make clean -C /root/Userland
docker exec -it TPE_SO chmod 777 /root/Image/x64BareBonesImage.qcow2
