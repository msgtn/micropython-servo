# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/ms/Developer/pico/micropython-servo/cpp/build/_deps/picotool-src"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/_deps/picotool-build"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/_deps"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/tmp"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/src/picotoolBuild-stamp"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/src"
  "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/src/picotoolBuild-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/src/picotoolBuild-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/ms/Developer/pico/micropython-servo/cpp/build/picotool/src/picotoolBuild-stamp${cfgdir}") # cfgdir has leading slash
endif()
