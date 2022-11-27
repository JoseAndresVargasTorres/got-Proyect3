# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-src"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-build"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix/tmp"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix/src"
  "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/josev/Downloads/Got-SVN-master/cmake-build-debug/_deps/curl-subbuild/curl-populate-prefix/src/curl-populate-stamp/${subDir}")
endforeach()
