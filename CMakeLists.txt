# Add as a subdirectory SimpleRenderEngineProject using add_subdirectory
IF (NOT DEFINED all_libs)
    message( FATAL_ERROR "This CMakeLists.txt must be included using the command add_subdirectory(name)" )
ENDIF (NOT DEFINED all_libs)
# Ensure SimpleRenderEngineProject is up-to-date
IF (WIN32)
    IF (NOT DEFINED DLLFileList)
        message( FATAL_ERROR "The SimpleRenderEngineProject has been updated. Clone the project (https://github.com/mortennobel/SimpleRenderEngineProject) again the project again and start over." )
    ENDIF(NOT DEFINED DLLFileList)
ENDIF(WIN32)

# Add hpp and cpp files
file(GLOB Zombie
        "*.hpp"
        "*.h"
        "*.cpp"
        )

# Compile cpp files (from Zombie variable)
add_executable(GP_Zombie ${Zombie})
target_link_libraries(GP_Zombie ${all_libs})

#copying levels
file(GLOB LEVEL_FILES
	"levels/*.json"
)
file(COPY ${LEVEL_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/levels)
file(COPY ${LEVEL_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/levels)
file(COPY ${LEVEL_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/levels)

#copying spritesheet  files
file(GLOB SPRITESHEET_FILES
	"spritesheets/*.json"
	"spritesheets/*.png"
)
file(COPY ${SPRITESHEET_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/spritesheets)
file(COPY ${SPRITESHEET_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/spritesheets)
file(COPY ${SPRITESHEET_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/spritesheets)

#copying audio files
file(GLOB AUDIO_FILES
	"audio/*.wav"
)
file(COPY ${AUDIO_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/audio)
file(COPY ${AUDIO_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Debug/audio)
file(COPY ${AUDIO_FILES} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Release/audio)


IF (WIN32)
    # Copy dll files to ${CMAKE_CURRENT_BINARY_DIR}/Debug
    file(COPY ${DLLFileList} DESTINATION Debug)
    file(COPY ${DLLFileList} DESTINATION Release)
    file(COPY ${DLLFileList} DESTINATION .)

    # Set working directory to ${CMAKE_CURRENT_BINARY_DIR}/Debug
    set_target_properties(GP_Zombie PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Debug)
ENDIF(WIN32)
