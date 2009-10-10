cmake_minimum_required(VERSION 2.6)

add_avr_executable(
    powerbar
    main.cpp 
    relay_controller.cpp relay_controller.hpp 
    user_interface.cpp user_interface.hpp
    command_handler.cpp command_handler.hpp
)


