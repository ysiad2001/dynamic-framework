add_executable(dynamic_framework

        )

target_compile_definitions(dynamic_framework PRIVATE
        PICO_DISABLE_SHARED_IRQ_HANDLERS=1
        PICO_TIME_DEFAULT_ALARM_POOL_DISABLED=1)


target_sources(dynamic_framework PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/main.c       
        ${CMAKE_CURRENT_LIST_DIR}/timer_functions.c
        ${CMAKE_CURRENT_LIST_DIR}/task0.c
        ${CMAKE_CURRENT_LIST_DIR}/task1.c
        ${CMAKE_CURRENT_LIST_DIR}/task2.c
        ${CMAKE_CURRENT_LIST_DIR}/task3.c
        ${CMAKE_CURRENT_LIST_DIR}/task4.c
        ${CMAKE_CURRENT_LIST_DIR}/task5.c
        ${CMAKE_CURRENT_LIST_DIR}/printout.c
        )

# Add pico_multicore which is required for multicore functionality
target_link_libraries(dynamic_framework
        pico_stdlib
        pico_multicore  
        hardware_structs 
        hardware_regs )

set_target_properties(dynamic_framework PROPERTIES PICO_TARGET_LINKER_SCRIPT 
        ${CMAKE_SOURCE_DIR}/linker.ld)

    
# create map/bin/hex file etc.
pico_add_extra_outputs(dynamic_framework)
