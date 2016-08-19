if(CMAKE_HOST_APPLE)
  find_program(BREW_COMMAND brew)
  execute_process(
    COMMAND ${BREW_COMMAND} --prefix valgrind
