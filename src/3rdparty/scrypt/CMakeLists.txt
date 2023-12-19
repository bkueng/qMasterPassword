set(SCRYPT_WINDOWS_LIB scrypt-windows)
set(SCRYPT_WINDOWS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/scrypt-windows)
set(SCRYPT_SOURCE_DIR ${SCRYPT_WINDOWS_DIR}/scrypt-1.1.6)

# static library implementation
add_library(
    ${SCRYPT_WINDOWS_LIB} STATIC
    ${SCRYPT_SOURCE_DIR}/lib/crypto/crypto_scrypt-sse.c
    ${SCRYPT_SOURCE_DIR}/lib/crypto/sha256.c ${SCRYPT_WINDOWS_DIR}/win/mman.c)
target_compile_definitions(
    ${SCRYPT_WINDOWS_LIB}
    PRIVATE CONFIG_H_FILE="${SCRYPT_WINDOWS_DIR}/config.h")
target_include_directories(
    ${SCRYPT_WINDOWS_LIB}
    PRIVATE ${SCRYPT_SOURCE_DIR} ${SCRYPT_SOURCE_DIR}/lib/crypto
            ${SCRYPT_SOURCE_DIR}/lib/scryptenc ${SCRYPT_SOURCE_DIR}/lib/util
            ${SCRYPT_WINDOWS_DIR}/win/include)
# disable Qt features for static library code
set_target_properties(${SCRYPT_WINDOWS_LIB} PROPERTIES AUTOMOC OFF AUTOUIC OFF)

# static library interface
target_include_directories(${TARGET} PRIVATE ${SCRYPT_SOURCE_DIR}/lib)
target_link_libraries(${TARGET} PRIVATE ${SCRYPT_WINDOWS_LIB})
