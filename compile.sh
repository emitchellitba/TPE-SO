docker start TPE_SO
docker exec -it TPE_SO make clean -C /root/Toolchain
docker exec -it TPE_SO make clean -C /root/
docker exec -it TPE_SO make -C /root/Toolchain
docker exec -it TPE_SO make -C /root/
docker exec -it TPE_SO chmod 777 /root/Image/x64BareBonesImage.qcow2
docker stop TPE_SO