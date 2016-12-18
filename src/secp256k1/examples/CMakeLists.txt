function(add_example name)
  set(target_name ${name}_example)
  add_executable(${target_name} ${name}.c)
