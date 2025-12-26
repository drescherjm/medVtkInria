vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO drescherjm/QtBasicUtils
    REF d387f37df12e25d8852d677abecfd450993031a5
    SHA512 94e648cb72063aa79509e05659f106088d95e8e77450d6375c5d1a19462df1a63652eaed2a6f5476db99d8410f2022134e06b5e79c5635c34df88a3edd28131f
)

set(CONFIGURE_OPTIONS)

if("qt5" IN_LIST FEATURES AND "qt6" IN_LIST FEATURES)
    message(FATAL_ERROR "Features 'qt5' and 'qt6' cannot be used together.")
elseif("qt5" IN_LIST FEATURES)
    list(APPEND CONFIGURE_OPTIONS -DSELECT_QT_VERSION=Qt5)
elseif("qt6" IN_LIST FEATURES)
    list(APPEND CONFIGURE_OPTIONS -DSELECT_QT_VERSION=Qt6)
else()
    message(FATAL_ERROR "Either 'qt5' or 'qt6' must be enabled.")
endif()

vcpkg_configure_cmake(
    SOURCE_PATH ${SOURCE_PATH}
    PREFER_NINJA
    OPTIONS ${CONFIGURE_OPTIONS}
)

vcpkg_cmake_build()
vcpkg_cmake_install()

# Fixup cmake config locations and paths. This is essential for vcpkg to setup the SimpleLibraryExampleConfig.cmake file 
# and setup each configuration for multiconfig generators like Visual Studio 2022. 
# Remember that vcpkg builds the Debug and Release config then maps the other configs: MinSizeRel, RelWithDebInfo to
# use the binaries created for the Release config. 
vcpkg_cmake_config_fixup(PACKAGE_NAME QtBasicUtils CONFIG_PATH lib/cmake/QtBasicUtils)

# Install license into share/${PORT}/
file(INSTALL "${SOURCE_PATH}/LICENSE" DESTINATION "${CURRENT_PACKAGES_DIR}/share/${PORT}")
