cmake_minimum_required(VERSION 3.14)
project(my_project)

set(CMAKE_BUILD_TYPE Debug)

# GoogleTest requires at least C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()


# ------------ TESTS ------------ 
add_executable(
  math_test 
  math_test.cc
  ../src/utils/math.cpp
)
target_link_libraries(
  math_test 
  GTest::gtest_main
)

add_executable(
  ray_test
  ray_test.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/pattern.cpp
)
target_link_libraries(
  ray_test
  GTest::gtest_main
)

add_executable(
  screen_test
  screen_test.cc
  ../src/utils/math.cpp
  ../src/screen/canvas.cpp
)
target_link_libraries(
  screen_test
  GTest::gtest_main
)

add_executable(
  shape_test 
  shape_test.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/utils/pattern.cpp
)
target_link_libraries(
  shape_test
  GTest::gtest_main
)

add_executable(
  world_test 
  world_test.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/scene/world.cpp
  ../src/scene/camera.cpp
  ../src/screen/canvas.cpp
  ../src/utils/pattern.cpp
)
target_link_libraries(
  world_test
  GTest::gtest_main
)

add_executable(
  unit_tests 
  unit_tests.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/scene/world.cpp
  ../src/scene/camera.cpp
  ../src/screen/canvas.cpp
  ../src/utils/pattern.cpp
)
target_link_libraries(
  unit_tests 
  GTest::gtest_main
)

# -------------------------

add_executable(
  tick
  tick.cpp
  ../src/utils/math.cpp
  ../src/screen/canvas.cpp
)
add_executable(
  clock
  clock_test.cc
  ../src/utils/math.cpp
  ../src/screen/canvas.cpp
)
add_executable(
  rtSphere
  rtSphere_test.cc
  ../src/utils/math.cpp
  ../src/screen/canvas.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/utils/pattern.cpp
)
add_executable(
  first_scene 
  ./renders/first_scene.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/scene/world.cpp
  ../src/scene/camera.cpp
  ../src/screen/canvas.cpp
  ../src/utils/pattern.cpp
)

add_executable(
  plane_scene 
  ./renders/plane_scene.cc
  ../src/utils/math.cpp
  ../src/utils/ray.cpp
  ../src/utils/shape.cpp
  ../src/utils/light.cpp
  ../src/scene/world.cpp
  ../src/scene/camera.cpp
  ../src/screen/canvas.cpp
  ../src/utils/pattern.cpp
)


include(GoogleTest)
gtest_discover_tests(math_test)
gtest_discover_tests(math_test)
gtest_discover_tests(ray_test)
gtest_discover_tests(screen_test)
gtest_discover_tests(shape_test)
gtest_discover_tests(world_test)
gtest_discover_tests(unit_tests)