cmake_minimum_required(VERSION 3.10)
project(SRTCN_Protokol C)
set(CMAKE_C_STANDARD 99)

# Debug sembolleri ve optimizasyon seviyesi
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/externa_libs
)

# Kaynak dosyaları
set(SRC_FILES
    src/crypto.c
    src/endian_utils.c
    src/message.c
    src/secure_codec.c
    src/srtcn.c
    src/platform/platform_windows.c
    externa_libs/aes.c
    externa_libs/hmac_sha256.c
    externa_libs/sha256.c
)

# Kütüphane oluştur
add_library(srtcn STATIC ${SRC_FILES})

# Test uygulamaları
add_executable(pub test/pub.c)
target_link_libraries(pub srtcn)

add_executable(sub test/sub.c)
target_link_libraries(sub srtcn)

# Platforma özel ayarlar
if(MSVC)
    add_compile_options(/utf-8 /Zi /Od)  # MSVC debug ayarları
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} /DEBUG")
else()
    add_compile_options(-Wall -Wextra)   # GCC/Clang uyarıları
endif()

# Debug mesajları için ek tanım
add_definitions(-DDEBUG_MODE=1)