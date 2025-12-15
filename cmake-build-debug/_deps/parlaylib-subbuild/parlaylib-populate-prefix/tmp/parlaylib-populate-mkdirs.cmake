# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-src"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-build"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/tmp"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/src/parlaylib-populate-stamp"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/src"
  "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/src/parlaylib-populate-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/src/parlaylib-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "C:/Users/paska/CLionProjects/parallel_bfs/cmake-build-debug/_deps/parlaylib-subbuild/parlaylib-populate-prefix/src/parlaylib-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
