# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "D:/esp/esp-idf/components/bootloader/subproject"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/tmp"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/src/bootloader-stamp"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/src"
  "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/STUDY/GREB/9_Bluetooth/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
