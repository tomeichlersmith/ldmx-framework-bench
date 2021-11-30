find_package(Boost REQUIRED COMPONENTS unit_test_framework)

add_executable(test_executable
  main_test.cpp
  dataset.cpp
  )
  
target_link_libraries(test_executable 
  PRIVATE Boost::unit_test_framework 
          fire::h5)

# insert running of test_executable as the 'make test' target
add_custom_target(test
  COMMAND $<TARGET_FILE:test_executable>
  COMMENT "Run the unit test executable"
)
