add_executable(test test.c)
set_property(TARGET test PROPERTY C_STANDARD 99)
target_link_libraries(test gn)
