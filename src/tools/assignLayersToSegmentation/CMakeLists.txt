add_executable(assignLayersToSegmentation assignLayersToSegmentation.cc)

if (BUILD_STATIC_TOOLS)
  target_link_libraries(assignLayersToSegmentation PRIVATE "-static")
  target_link_libraries(assignLayersToSegmentation
    PRIVATE IRoCS_static_tools segmentation_static_tools
    cmdline_static_tools)
elseif (BUILD_STATIC_LIBS)
  target_link_libraries(assignLayersToSegmentation
    PRIVATE IRoCS_static segmentation_static cmdline_static)
elseif (BUILD_SHARED_LIBS)
  target_link_libraries(assignLayersToSegmentation
    PRIVATE IRoCS segmentation cmdline)
endif()

install(TARGETS assignLayersToSegmentation RUNTIME DESTINATION bin)
