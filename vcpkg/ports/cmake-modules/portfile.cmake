vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO drescherjm/cmake-modules
    REF 2b204b28c7259984627ca4ee453537946a2aefe3
    SHA512 b07a57004f521fa2969a6592ca65a677d46f5a2be70f6121919f1937af221cee904096b5fcfa3eed4bcd8f05f2b8021a963f84c7a0e6aa129527fb3626248208
)

# Install the modules and config file
file(INSTALL ${SOURCE_PATH}/Modules DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-modules)
file(INSTALL ${SOURCE_PATH}/Scripts DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-modules)
file(INSTALL ${SOURCE_PATH}/cmake/cmake-modulesConfig.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-modules)
file(INSTALL ${SOURCE_PATH}/cmake/cmake-modulesTargets.cmake DESTINATION ${CURRENT_PACKAGES_DIR}/share/cmake-modules)
