cmake_minimum_required(VERSION 3.9 FATAL_ERROR)

project(json_test CXX C)
set(CMAKE_C_FLAGS $ENV{CFLAGS})
set(CMAKE_CXX_STANDARD 17)

find_package(MPI REQUIRED)
find_package(ROOT REQUIRED COMPONENTS)
find_package(nlohmann_json REQUIRED)

message(STATUS "C++ compiler is :${CMAKE_CXX_COMPILER}")


list(APPEND EXTRA_INCLUDES "/Users/abashyal/Argonne_Stuff/main_root_serialization/tbb/2020_U3/include")

list(APPEND EXTRA_LIBDIR "/Users/abashyal/Argonne_Stuff/main_root_serialization/tbb/2020_U3/lib")

include_directories (SYSTEM
		     ${EXTRA_INCLUDES}
		     ${MPI_INCLUDE_DIRS}
		     ${MPI_CXX_INCLUDE_DIRS}
		     ${ROOT_INCLUDE_DIRS}
		     )

set (libraries ${libraries}
    	       ${ROOT_LIBRARIES}
	       ${MPI_CXX_LIBRARIES}
	       ROOT::Core
	       ROOT::RIO
	       ROOT::Tree
	       ROOT::Hist
	       )

add_executable(test_json test_json.cpp)
target_link_directories(test_json PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_json PRIVATE nlohmann_json::nlohmann_json tbb ${libraries})


add_executable(test_tbb test_tbb.cpp)
target_link_directories(test_tbb PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_tbb PRIVATE nlohmann_json::nlohmann_json tbb ${libraries})

add_executable(test_arena test_arena.cpp)
target_link_directories(test_arena PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_arena PRIVATE nlohmann_json::nlohmann_json tbb ${libraries})


add_executable(test_mpi test_mpi.cc)
target_link_directories(test_mpi PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_mpi PRIVATE nlohmann_json::nlohmann_json tbb ${libraries})

add_executable(test_atomic test_atomic.cpp)
target_link_directories(test_atomic PUBLIC ${EXTRA_LIBDIR})
target_link_libraries(test_atomic PRIVATE nlohmann_json::nlohmann_json tbb ${libraries})

#add_executable(test_functor test_functor.cpp)
#target_link_directories(test_functor PUBLIC ${EXTRA_LIBDIR})
#target_link_libraries(test_functor PRIVATE tbb ${libraries})
