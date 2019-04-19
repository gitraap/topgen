cmake_minimum_required (VERSION 2.6)

# Project title
project (libtopgen)

# Manual assignation
#SET(TOPGEN_PBGL "yes")

# Set the compiler
IF(TOPGEN_PBGL)
    message(STATUS "Enabled the PBGL support")
    SET(CMAKE_CXX_COMPILER "mpic++")
else()
    message(STATUS "Enabled the BGL support. Use -DTOPGEN_PBGL to enable the PBGL support")
    SET(CMAKE_CXX_COMPILER "g++")
endif()

SET(CMAKE_CXX_FLAGS "-fopenmp -std=c++11 -Wall -fPIC -Wno-deprecated -Wwrite-strings -no-pie")

# Installation by default
IF (NOT CMAKE_INSTALL_PREFIX)
    SET(CMAKE_INSTALL_PREFIX /usr/local/topgen)
ENDIF()
message(STATUS "Install to ${CMAKE_INSTALL_PREFIX}")

   
# Include topgen headers
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Include MPI libraries
IF(TOPGEN_PBGL)

    # Include boost libraries
    FIND_PACKAGE( Boost 1.65 COMPONENTS program_options serialization system graph_parallel mpi REQUIRED )
    IF(Boost_FOUND)
        message(STATUS "Found Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
        message(STATUS "Found Boost_LIBRARIES: ${Boost_LIBRARIES}")
        INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    else()
        message(WARNING "Boost NOT found")
    ENDIF()

    #FIND_PACKAGE(MPI REQUIRED)
    #IF (MPI_FOUND)
    #    message(STATUS "Found MPI_INCLUDE_PATH: ${MPI_INCLUDE_PATH}")
    #    INCLUDE_DIRECTORIES(${MPI_INCLUDE_PATH})
    #ENDIF()
ELSE()
    # Include boost libraries
    FIND_PACKAGE( Boost 1.65 COMPONENTS program_options serialization system graph REQUIRED )
    IF(Boost_FOUND)
        message(STATUS "Found Boost_INCLUDE_DIR: ${Boost_INCLUDE_DIR}")
        message(STATUS "Found Boost_LIBRARIES: ${Boost_LIBRARIES}")
        INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )
    else()
        message(WARNING "Boost NOT found")
    ENDIF()

ENDIF()

# Enable GTest tests
enable_testing()
find_package( GTest REQUIRED )

if(GTEST_FOUND)
    message(STATUS "Found GTEST_INCLUDE_DIRS: ${GTEST_INCLUDE_DIRS}")
    message(STATUS "Found GTEST_LIBRARIES: ${GTEST_LIBRARIES}")

    include_directories( ${GTEST_INCLUDE_DIRS})
    SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# Include source files
file(GLOB_RECURSE LIBTOPGEN_HEADERS "include/" *.hpp)
file(GLOB_RECURSE LIBTOPGEN_SOURCES "src/" *.cpp)
file(GLOB TESTER_SOURCES "test/*.cpp")

# Build library
add_library(topgen ${LIBTOPGEN_SOURCES})

if(TOPGEN_PBGL)
    #PBGL linking
    target_link_libraries(topgen m ${Boost_LIBRARIES}) 
else()
    #BGL linking
    target_link_libraries(topgen m ${Boost_LIBRARIES} pthread gomp)
endif()


install(TARGETS topgen DESTINATION ${CMAKE_INSTALL_PREFIX}/lib EXPORT topgen)

install(DIRECTORY "include/" # source directory
        DESTINATION "${CMAKE_INSTALL_PREFIX}/include" # target directory
        FILES_MATCHING # install only matched files
        PATTERN "*.hpp" # select header files
        )

# Build unit tests
enable_testing()
include_directories(${GTEST_INCLUDE_DIRS}/include)
include_directories("include/")

add_executable( tester ${TESTER_SOURCES} )
target_link_libraries(tester ${GTEST_LIBRARIES} topgen)
add_test( tester RUN_ALL_TESTS )
install(TARGETS tester DESTINATION ${CMAKE_INSTALL_PREFIX}/bin)
