How to run & debug kinetis using JLinkGDBServer

1)  Run gdb server: "JLinkGDBServer.exe -select USB -device MK22FN512xxx12 -if JTAG -speed auto"
2)  Run gdb client: "arm-none-eabi-gdb.exe -x twrk22120m_gdb.txt"
3)  Run one of LOAD_XXXXX functions defined in twrk22120m_gdb.txt
    using application path as argument.
       f.i "LOAD_FLASH app/path/intflash_debug/hello.elf"
4)  Type "continue" to run application

Note:
    - Only work with Segger Jlink 4.82 or higher
    - Ssame elf file may be flashed only once
    - Verified with JLINK JLinkARM_V482, codesourcery arm-2013.q3-arm-none-eabi
