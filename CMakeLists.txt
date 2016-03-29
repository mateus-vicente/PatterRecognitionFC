cmake_minimum_required(VERSION 2.8)
project( PixelShop )
find_package( OpenCV )

find_package(ROOT COMPONENTS EG Eve Geom Gui GuiHtml GenVector Hist Physics Matrix Graf RIO Tree Gpad RGL MathCore)
include(${ROOT_USE_FILE})

include_directories( ${OpenCV_INCLUDE_DIRS} )
add_executable( PixelShop PixelShop.cpp )
target_link_libraries( PixelShop ${OpenCV_LIBS} ${ROOT_LIBRARIES} ${ROOT_COMPONENT_LIBRARIES})
