cmake_minimum_required(VERSION 3.16)

project(libsecp256k1
  # The package (a.k.a. release) version is based on semantic versioning 2.0.0 of
  # the API. All changes in experimental modules are treated as
  # backwards-compatible and therefore at most increase the minor version.
  VERSION 0.5.1
  DESCRIPTION "Optimized C library for ECDSA signatures and secret/public key operations on curve secp256k1."
  HOMEPAGE_URL "https://github.com/bitcoin-core/secp256k1"
  LANGUAGES C
)

if(CMAKE_VERSION VERSION_LESS 3.21)
  # Emulates CMake 3.21+ behavior.
  if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(PROJECT_IS_TOP_LEVEL ON)
    set(${PROJECT_NAME}_IS_TOP_LEVEL ON)
  else()
    set(PROJECT_IS_TOP_LEVEL OFF)
    set(${PROJECT_NAME}_IS_TOP_LEVEL OFF)
  endif()
endif()

# The library version is based on libtool versioning of the ABI. The set of
# rules for updating the version can be found here:
# https://www.gnu.org/software/libtool/manual/html_node/Updating-version-info.html
# All changes in experimental modules are treated as if they don't affect the
# interface and therefore only increase the revision.
set(${PROJECT_NAME}_LIB_VERSION_CURRENT 4)
set(${PROJECT_NAME}_LIB_VERSION_REVISION 1)
set(${PROJECT_NAME}_LIB_VERSION_AGE 2)

set(CMAKE_C_STANDARD 90)
set(CMAKE_C_EXTENSIONS OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

