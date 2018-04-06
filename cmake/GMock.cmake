find_package(Threads REQUIRED)

include(ExternalProject)

ExternalProject_Add(
    gtest-project
    URL https://github.com/google/googletest/archive/master.zip
    PREFIX ${CMAKE_CURRENT_BINARY_DIR}/gtest
    # Disable install step
    INSTALL_COMMAND "")
ExternalProject_Get_Property(gtest-project source_dir binary_dir)

add_library(libgtest IMPORTED STATIC GLOBAL)
add_dependencies(libgtest gtest-project)
file(MAKE_DIRECTORY "${source_dir}/googletest/include")
set_target_properties(libgtest PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googletest/include"
    INTERFACE_LINK_LIBRARIES "${CMAKE_THREAD_LIBS_INIT}")

add_library(libgmock IMPORTED STATIC GLOBAL)
add_dependencies(libgmock libgtest)
file(MAKE_DIRECTORY "${source_dir}/googlemock/include")
set_target_properties(libgmock PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES "${source_dir}/googlemock/include"
    INTERFACE_LINK_LIBRARIES "libgtest")

add_library(libgmock_main IMPORTED STATIC GLOBAL)
add_dependencies(libgmock_main libgmock)
set_target_properties(libgmock_main PROPERTIES
    INTERFACE_LINK_LIBRARIES "libgmock")

if (CMAKE_GENERATOR MATCHES "Xcode")
    set_target_properties(libgtest PROPERTIES
        IMPORTED_LOCATION_RELEASE "${binary_dir}/googlemock/gtest/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a"
        IMPORTED_LOCATION_DEBUG "${binary_dir}/googlemock/gtest/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gtestd.a")
    set_target_properties(libgmock PROPERTIES
        IMPORTED_LOCATION_RELEASE "${binary_dir}/googlemock/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a"
        IMPORTED_LOCATION_DEBUG "${binary_dir}/googlemock/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gmockd.a")
    set_target_properties(libgmock_main PROPERTIES
        IMPORTED_LOCATION_RELEASE "${binary_dir}/googlemock/Release/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a"
        IMPORTED_LOCATION_DEBUG "${binary_dir}/googlemock/Debug/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_maind.a")
else ()
    set_target_properties(libgtest PROPERTIES
        IMPORTED_LOCATION "${binary_dir}/googlemock/gtest/${CMAKE_FIND_LIBRARY_PREFIXES}gtest.a")
    set_target_properties(libgmock PROPERTIES
        IMPORTED_LOCATION "${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock.a")
    set_target_properties(libgmock_main PROPERTIES
        IMPORTED_LOCATION "${binary_dir}/googlemock/${CMAKE_FIND_LIBRARY_PREFIXES}gmock_main.a")
endif()
