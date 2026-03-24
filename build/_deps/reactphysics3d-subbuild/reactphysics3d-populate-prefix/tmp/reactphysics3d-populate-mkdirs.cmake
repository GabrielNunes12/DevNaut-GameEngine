# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file LICENSE.rst or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION ${CMAKE_VERSION}) # this file comes with cmake

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "F:/C++GameEngine/build/_deps/reactphysics3d-src")
  file(MAKE_DIRECTORY "F:/C++GameEngine/build/_deps/reactphysics3d-src")
endif()
file(MAKE_DIRECTORY
  "F:/C++GameEngine/build/_deps/reactphysics3d-build"
  "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix"
  "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/tmp"
  "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp"
  "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src"
  "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp"
)

set(configSubDirs Debug)
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "F:/C++GameEngine/build/_deps/reactphysics3d-subbuild/reactphysics3d-populate-prefix/src/reactphysics3d-populate-stamp${cfgdir}") # cfgdir has leading slash
endif()
