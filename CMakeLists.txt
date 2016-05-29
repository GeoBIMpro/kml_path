cmake_minimum_required(VERSION 2.8.3)
project(kml_path)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
  nav_msgs
)
find_package(LibXml2)

catkin_package(
  CATKIN_DEPENDS
  roscpp
  sensor_msgs
  nav_msgs
)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
	message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

include_directories(
  ${catkin_INCLUDE_DIRS}
  ${LIBXML2_INCLUDE_DIR}
)

add_executable(kml_path src/kml_path.cpp src/kmlparser.c src/wgs84.c)
target_link_libraries(kml_path ${catkin_LIBRARIES} ${LIBXML2_LIBRARIES})

