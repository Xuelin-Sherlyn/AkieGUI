cmake_minimum_required(VERSION 3.20)

project(AkieGUI LANGUAGES C)

set(AKIEGUI_PORT_INCLUDE_DIR "" CACHE PATH "Directory containing a platform-specific akiegui_port.h")

add_library(AkieGUI STATIC
	Core/Src/akiegui_core.c
	Core/Src/akiegui_memory.c
	Common/Src/akiegui_draw.c
	Common/Src/akiegui_touch.c
	Widget/akiegui_widget.c
	Widget/Button/akiegui_button.c
	Widget/Image/akiegui_image.c
	Widget/Label/akiegui_label.c
	Widget/Progress/akiegui_progress.c
	Fonts/akiegui_font_ascii.c
	Fonts/akiegui_font_chinese.c
)

add_library(AkieGUI::AkieGUI ALIAS AkieGUI)

if(AKIEGUI_PORT_INCLUDE_DIR)
	target_include_directories(AkieGUI BEFORE PUBLIC
		"${AKIEGUI_PORT_INCLUDE_DIR}"
	)
endif()

target_include_directories(AkieGUI PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}"
	"${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc"
	"${CMAKE_CURRENT_SOURCE_DIR}/Common/Inc"
	"${CMAKE_CURRENT_SOURCE_DIR}/Widget"
	"${CMAKE_CURRENT_SOURCE_DIR}/Widget/Button"
	"${CMAKE_CURRENT_SOURCE_DIR}/Widget/Image"
	"${CMAKE_CURRENT_SOURCE_DIR}/Widget/Label"
	"${CMAKE_CURRENT_SOURCE_DIR}/Widget/Progress"
	"${CMAKE_CURRENT_SOURCE_DIR}/Fonts"
)

target_compile_features(AkieGUI PUBLIC c_std_11)
