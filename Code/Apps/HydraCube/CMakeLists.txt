set(SOURCES
  Source/Engine.cpp
  Source/Engine.hpp
  Source/HydraCube.cpp
  Source/MaterialHelpers.cpp
  Source/MaterialHelpers.hpp
  Source/MeshHelpers.cpp
  Source/MeshHelpers.hpp
  Source/Scene.cpp
  Source/Scene.hpp
  Source/Surface.cpp
  Source/Surface.hpp
)
source_group(src FILES ${SOURCES})

add_executable(HydraCube 
  ${SOURCES}
)
target_include_directories(HydraCube PRIVATE
   ${CMAKE_CURRENT_SOURCE_DIR}/Source
)
target_link_libraries(HydraCube PRIVATE

  hgi
  hd
  hdx
  
  glfw
  OpenGL::GL
)
set_target_properties(HydraCube PROPERTIES
   FOLDER "Apps"
   VS_GLOBAL_EnableClangTidyCodeAnalysis true
   VS_GLOBAL_EnableMicrosoftCodeAnalysis false
)

# Set environment variables for debugging in Visual Studio
# Set PATH so that various OpenUSD DLLs are found
# Set TF_DEBUG to filter out debug messages from Hydra (Enable everything, then disable noisy ones)
set_target_properties(HydraCube 
   PROPERTIES VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_LINKER_FILE_DIR:hd>;$<TARGET_LINKER_FILE_DIR:hd>/../bin\nTF_DEBUG=HGI* TF* HD* -HGIGL_DEBUG_FRAMEBUFFER_CACHE -HDST_DUMP_SHADER_SOURCE -HDST_DRAW_BATCH -HDST_DRAW -HDST_LOG_* -HD_SYNC_* -HD_DIRTY_* -HD_ENGINE_PHASE_INFO -HD_BUFFER_ARRAY_RANGE_CLEANED"
)
