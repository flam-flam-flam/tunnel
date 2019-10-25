PROJECT(c_client)
set(CMAKE_C_FLAGS "-O0 -g")
#SET(CMAKE_SYSTEM_NAME Linux) #mips
#SET(CMAKE_C_COMPILER "/home/buildroot/buildroot-gcc463/usr/bin/mipsel-linux-gcc") #mips
LINK_DIRECTORIES(/home/gaochuang/c_client/lib/linux)

#LINK_DIRECTORIES(libs) #mips
SET(SRC_LIST at_client.c ae_select.c ae.c at_control.c operate.c socket.c ssl.c cJSON.c zmalloc.c list.c config.c)
ADD_EXECUTABLE(main ${SRC_LIST})
#mips
#include_directories("/home/buildroot/buildroot-gcc463/usr/lib/mipsk/include")
#LINK_DIRECTORIES("/home/buildroot/buildroot-gcc463/usr/lib/mipsk/lib")
#include_directories("/home/gaochuang/c_client/lib/mips/include")
#target_link_libraries(main /home/buildroot/buildroot-gcc463/usr/lib/mipsk/lib/libssl.so )
#target_link_libraries(main /home/buildroot/buildroot-gcc463/usr/lib/mipsk/lib/libcrypto.so )
#target_link_libraries(main libzlog.so )
#linux
target_link_libraries(main -lssl )
target_link_libraries(main -lcrypto )
target_link_libraries(main -lzlog )
