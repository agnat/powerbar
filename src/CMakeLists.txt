
add_avr_executable(
    powerbar
    main.cpp 
    relay_controller.cpp relay_controller.hpp 
    user_interface.cpp user_interface.hpp
    connection_handler.cpp connection_handler.hpp
    model.cpp model.hpp
)

