# idf_component_register(SRCS "main.cpp" "${CMAKE_CURRENT_BINARY_DIR}/Flute/Flute.cpp"
#                        INCLUDE_DIRS ${CMAKE_CURRENT_BINARY_DIR})

# add_custom_command(OUTPUT Flute/Flute.h Flute/Flute.cpp
#      COMMAND faust2esp32 -es8388 -lib ${COMPONENT_DIR}/Flute.dsp && unzip ${CMAKE_CURRENT_BINARY_DIR}/Flute.zip
#      DEPENDS ${COMPONENT_DIR}/Flute.dsp
#      COMMENT "Compiling faust..."
#      VERBATIM)

# add_custom_target(Flute DEPENDS Flute/Flute.h Flute/Flute.cpp Flute/es8388.cpp)
# add_dependencies(${COMPONENT_LIB} Flute)

# set_property(DIRECTORY "${COMPONENT_DIR}" APPEND PROPERTY
#      ADDITIONAL_MAKE_CLEAN_FILES Flute/Flute.h Flute/Flute.cpp Flute/es8388.cpp)

idf_component_register(SRCS "main.cpp" "Flute.cpp"
                       INCLUDE_DIRS ./)
