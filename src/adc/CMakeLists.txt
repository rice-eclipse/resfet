# Create the adc library
add_library(adc STATIC adc.cpp)
target_link_libraries(adc bcm2835)
