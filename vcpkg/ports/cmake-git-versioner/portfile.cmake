vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO symboxtra/cmake-GitVersioner
    REF 62f8fb66f61fb6575787a5d80d8182d2d60aeadc
    SHA512 138016093f73cff648cc66b27f5d720667cd8e0c46d683feed7a9023790535dd41ebed41af17d097360ccc30fca88a1cbd2a2116b96a898c35d23297b3d10682
)

# Install the modules and config file
file(INSTALL ${SOURCE_PATH}/DetectCI.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-git-versioner)
file(INSTALL ${SOURCE_PATH}/GitVersioner.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-git-versioner)
file(INSTALL ${SOURCE_PATH}/LICENSE.txt DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-git-versioner)

# Write the Config.cmake file
file(WRITE "${CURRENT_PACKAGES_DIR}/share/cmake-git-versioner/${PORT}Config.cmake" "
set(${PORT}_DIR ${CURRENT_PACKAGES_DIR}/share/cmake-git-versioner)
")