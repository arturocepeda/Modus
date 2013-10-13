set(LIBRARY_OUTPUT_PATH ${MODUS_LIBRARY_DIR})

# Specify targets and files
set(TARGET modus)
file(GLOB SRC *.cpp)
file(GLOB HDR *.h)

# Create the library
add_library(${TARGET} STATIC ${SRC} ${HDR})

# Define installation locations
install(TARGETS ${TARGET}
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
  )

install(FILES ${HDR} DESTINATION include)

