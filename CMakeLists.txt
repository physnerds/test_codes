cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(json_test CXX C)
set(CMAKE_C_FLAGS $ENV{CFLAGS})
set(CMAKE_CXX_STANDARD 17)

find_package(TBB REQUIRED)
find_package(MPI REQUIRED)
find_package(ROOT REQUIRED COMPONENTS)
find_package(nlohmann_json REQUIRED)

message(STATUS "C++ compiler is :${CMAKE_CXX_COMPILER}")

set(INCLUDE_DIR /opt/homebrew/include)
set(ONEAPI_DIR /opt/homebrew/include/oneapi)
set(TBB_LIB /opt/homebrew/lib/libtbb.so)

include_directories (SYSTEM
		     ${EXTRA_INCLUDES}
		     ${MPI_INCLUDE_DIRS}
		     ${MPI_CXX_INCLUDE_DIRS}
             ${INCLUDE_DIR}
             ${ONEAPI_DIR}
		     ${ROOT_INCLUDE_DIRS}
             ${EXTRA_INCLUDE_DIR}
                
		     )

set (libraries ${libraries}
    	       ${ROOT_LIBRARIES}
	       ${MPI_CXX_LIBRARIES}
           ${TBB_LIB}
	       ROOT::Core
	       ROOT::RIO
	       ROOT::Tree
	       ROOT::Hist
	       )

add_executable(test_json test_json.cpp)
target_link_directories(test_json PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_json PRIVATE nlohmann_json::nlohmann_json ${libraries})



#add_executable(test_functor test_functor.cpp)
#target_link_directories(test_functor PUBLIC ${EXTRA_LIBDIR})
#target_link_libraries(test_functor PRIVATE ${libraries})


add_executable(test_tuple test_tuple.cpp)
target_link_directories(test_tuple PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_tuple PRIVATE ${libraries})

add_executable(test_arraypointers test_arraypointers.cpp)
target_link_directories(test_arraypointers PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_arraypointers PRIVATE ${libraries})
