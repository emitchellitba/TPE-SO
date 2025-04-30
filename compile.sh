docker start TPE_Arqui
docker exec -it TPE_Arqui make clean -C /root/Toolchain
docker exec -it TPE_Arqui make clean -C /root/
docker exec -it TPE_Arqui make -C /root/Toolchain
docker exec -it TPE_Arqui make -C /root/
docker exec -it TPE_Arqui chmod 777 /root/Image/x64BareBonesImage.qcow2
docker stop TPE_Arqui