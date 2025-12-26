# This file works with vcpkg.json to support selecting either Qt5 or Qt6 using a vcpkg feature.
# When the feature selected changes this script will automatically unset the Qt cache variables
# so that you don't have conflicting dependencies between Qt5 and Qt6 in your application. 

set(_LAST_QT_VERSION "${SELECT_QT_VERSION}")
get_property(_cached_LAST_QT_VERSION CACHE LAST_QT_VERSION PROPERTY VALUE)

set(SELECT_QT_VERSION "Qt6" CACHE STRING "Please select the Qt version.")
set_property(CACHE SELECT_QT_VERSION PROPERTY STRINGS "Qt5" "Qt6")

# With vcpkg features must be lowercase so we convert to qt5 and qt6 with the following:
string(TOLOWER "${SELECT_QT_VERSION}" VCPKG_FEATURE_NAME)

#Tell vcpkg the feature to use!
list(APPEND VCPKG_MANIFEST_FEATURES ${VCPKG_FEATURE_NAME})

MESSAGE(STATUS "Current=${SELECT_QT_VERSION} Last=${_LAST_QT_VERSION}")

# If we changed Qt versions we have to unset the cache variables!
if(DEFINED _cached_LAST_QT_VERSION AND NOT _LAST_QT_VERSION STREQUAL _cached_LAST_QT_VERSION)
    message(STATUS "Qt version has changed from '${_cached_LAST_QT_VERSION}' to '${_LAST_QT_VERSION}'")
   	#Clear any Qt* variables from the previous.
	get_cmake_property(_cacheVars CACHE_VARIABLES)
	foreach(_var ${_cacheVars})
		if(_var MATCHES "^Qt" OR _var MATCHES "DEPLOYQT_EXECUTABLE$")
			message(STATUS "Unsetting cache variable: ${_var}")
			unset(${_var} CACHE)
		endif()
	endforeach()
endif()

set(LAST_QT_VERSION "${SELECT_QT_VERSION}" CACHE INTERNAL "Last configured value of SELECT_QT_VERSION")