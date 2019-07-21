cmake_minimum_required (VERSION 3.0)
project (allocator VERSION 1.0.1)

SET(CPACK_GENERATOR "DEB")
SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "Aleksei Zakharov")

add_executable(allocator allocator.cpp)
INSTALL(TARGETS allocator RUNTIME DESTINATION bin)
INCLUDE(CPack)
