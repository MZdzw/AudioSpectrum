cmake_minimum_required(VERSION 3.22)

add_subdirectory(unity)
add_subdirectory(cmock)

add_subdirectory(Test_SpiBuffer)
add_subdirectory(Test_Ws2812bDriver)
add_subdirectory(Test_Animations)
add_subdirectory(Test_Communication)
add_subdirectory(Test_Activity)

add_test(NAME Activity COMMAND Test_Activity)
add_test(NAME Animations COMMAND Test_Animations)
add_test(NAME Communication COMMAND Test_Communication)
add_test(NAME spiBuffer COMMAND Test_SpiBuffer)
add_test(NAME Ws2812bDriver COMMAND Test_Ws2812bDriver)
