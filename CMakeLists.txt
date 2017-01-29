cmake_minimum_required(VERSION 3.5.0)
project(tMonitor)

enable_language(ASM)

IF(NOT CMAKE_CROSSCOMPILING)
    message(FATAL_ERROR "Cross compiling only. Please use -DCMAKE_TOOLCHAIN_FILE=/PATH/TO/TOOLCHAIN_FILE .")
ENDIF(NOT CMAKE_CROSSCOMPILING)

add_definitions(-DSTM32F1)

set(ATOMTHREADS ${CMAKE_SOURCE_DIR}/lib/onewire)
set(ATOMTHREADS_PORTROOT ${ATOMTHREADS}/ports/cortex-m)
set(KS0108_ROOT ${CMAKE_SOURCE_DIR}/lib/ks0108)

set(LIBOPENCM3_ROOT ${ATOMTHREADS_PORTROOT}/libopencm3)
set(ONEWIRE_ROOT ${CMAKE_SOURCE_DIR}/lib/onewire)

#file(GLOB_RECURSE USER_SOURCES src/*.c)

include_directories(
        ${LIBOPENCM3_ROOT}/include
        ${ATOMTHREADS}/kernel
        ${ATOMTHREADS_PORTROOT}
        ${ONEWIRE_ROOT}/src
        ${KS0108_ROOT}
        inc)

link_directories(
        ${LIBOPENCM3_ROOT}/lib
        ${ATOMTHREADS_PORTROOT}/build)

set(STM32F1_FLAGS "-mcpu=cortex-m3 -mthumb -mthumb-interwork -msoft-float")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall ${STM32F1_FLAGS} -Os -ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS "${CMAKE_C_FLAGS} -Wall -std=c++14 ${STM32F1_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -T ${CMAKE_SOURCE_DIR}/libopencm3.ld -lopencm3_stm32f1 -nostartfiles --specs=rdimon.specs -Wl,--gc-sections")


add_custom_target(libopencm3 make WORKING_DIRECTORY ${LIBOPENCM3_ROOT})
add_custom_target(atomthreads make WORKING_DIRECTORY ${ATOMTHREADS_PORTROOT})

function(add_bin_from_elf bin elf)
    add_custom_target(${bin}
            ALL ${CMAKE_OBJCOPY}
            -Obinary ${elf} ${bin} DEPENDS ${elf})
endfunction(add_bin_from_elf)

add_subdirectory(src)