# Must be included before CMAKE_INSTALL_INCLUDEDIR is used.
include(GNUInstallDirs)

add_library(secp256k1_precomputed OBJECT EXCLUDE_FROM_ALL
  precomputed_ecmult.c
  precomputed_ecmult_gen.c
)

# Add objects explicitly rather than linking to the object libs to keep them
# from being exported.
add_library(secp256k1 secp256k1.c $<TARGET_OBJECTS:secp256k1_precomputed>)

add_library(secp256k1_asm INTERFACE)
if(SECP256K1_ASM STREQUAL "arm32")
  add_library(secp256k1_asm_arm OBJECT EXCLUDE_FROM_ALL)
  target_sources(secp256k1_asm_arm PUBLIC
    asm/field_10x26_arm.s
