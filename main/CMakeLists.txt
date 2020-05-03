idf_component_register(SRCS "main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Flute/Flute.cpp"
                       INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR}
                       REQUIRES audio_sal audio_hal esp_dispatcher esp_peripherals display_service libmapper protocol_examples_common MapLooper)

add_custom_command(OUTPUT Flute/Flute.h Flute/Flute.cpp
     COMMAND faust2esp32 -lib ${COMPONENT_DIR}/Flute.dsp
     DEPENDS ${COMPONENT_DIR}/Flute.dsp
     COMMENT "Compiling faust..."
     VERBATIM)

add_custom_target(Flute DEPENDS Flute/Flute.h Flute/Flute.cpp)
add_dependencies(${COMPONENT_LIB} Flute)

set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
     ADDITIONAL_MAKE_CLEAN_FILES Flute/Flute.h Flute/Flute.cpp)

target_compile_options(${COMPONENT_LIB} PRIVATE -frtti)
