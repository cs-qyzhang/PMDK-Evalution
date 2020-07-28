cmake_minimum_required(VERSION 3.3)
project(PMDK_Evaluation)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

add_executable(pmem_test pmem.c)
target_link_libraries(pmem_test pmem)

add_executable(obj_test pmemobj.c)
target_link_libraries(obj_test pmemobj)

add_executable(tx_test obj_transaction.c)
target_link_libraries(tx_test pmemobj)

add_executable(cpp_test pmemobj-cpp.cc)
target_link_libraries(cpp_test pmemobj)

add_executable(obj_recover pmemobj-recover.c)
target_link_libraries(obj_recover pmemobj)