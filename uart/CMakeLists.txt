cmake_minimum_required(VERSION 3.10.1)

find_program(AVR_CC avr-gcc)
find_program(AVR_CXX avr-g++)
find_program(AVR_OBJCOPY avr-objcopy)
find_program(AVR_SIZE_TOOL avr-size)
find_program(AVR_OBJDUMP avr-objdump)
find_program(AVR_DUDE avrdude)

set(CMAKE_C_COMPILER ${AVR_CC})
set(CMAKE_CXX_COMPILER ${AVR_CXX})

set(AVR_MCU atmega8a)
set(AVR_MCU_DEFINE __AVR_ATmega8A__)
set(AVR_DUDE_MCU m8)
set(MCU_SPEED 1000000UL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 11)

function(avr_add_executable NAME)
  if(NOT ARGN)
    message(FATAL_ERROR "Missing target [${NAME}] objects")
  endif(NOT ARGN)


  set(FILE_ELF ${NAME}.elf)
  set(FILE_MAP ${NAME}.map)
  set(FILE_HEX ${NAME}.hex)
  set(FILE_LST ${NAME}.lst)

  add_executable(${FILE_ELF} ${ARGN})

  set_target_properties(${FILE_ELF} PROPERTIES
    COMPILE_FLAGS "-mmcu=${AVR_MCU} -D${AVR_MCU_DEFINE} -DF_CPU=${MCU_SPEED} -fpack-struct -fshort-enums -Wall  -pedantic-errors -funsigned-char -funsigned-bitfields -ffunction-sections -Os -save-temps"
    LINK_FLAGS "-mmcu=${AVR_MCU} -Wl,--gc-sections -mrelax -Wl,-Map,${FILE_MAP}"
  )

  target_include_directories(${FILE_ELF} PRIVATE "/home/memelord/git/avr-cmake")

  add_custom_command(
     OUTPUT ${FILE_HEX}
     COMMAND
        ${AVR_OBJCOPY} -j .text -j .data -O ihex ${FILE_ELF} ${FILE_HEX}
     COMMAND
        ${AVR_SIZE_TOOL} ${FILE_ELF}
     DEPENDS ${FILE_ELF}
  )

  add_custom_target(
     disassemble_${NAME}
     ${AVR_OBJDUMP} -h -S ${FILE_ELF} > ${FILE_LST}
     DEPENDS ${FILE_ELF}
  )

  add_custom_target(
    flash_${NAME}
    sudo ${AVR_DUDE} -c usbasp -p ${AVR_DUDE_MCU} -U flash:w:${FILE_HEX}
    DEPENDS ${FILE_HEX}
  )


  add_custom_target(
     ${NAME}
     ALL
     DEPENDS ${FILE_HEX} disassemble_${NAME}
  )

endfunction(avr_add_executable)

avr_add_executable(tx tx.cpp)
