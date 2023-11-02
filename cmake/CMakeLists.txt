cmake_minimum_required(VERSION 3.5)

if (WIN32)
	include(visualstudio.cmake NO_POLICY_SCOPE)
endif()

project("Krisp SDK Sample Apps")

# Set 'Release' or 'Debug'
set(CMAKE_BUILD_TYPE Debug)

get_filename_component(ROOT_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ROOT_DIR}/bin)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# is needed to get ${LIBSNDFILE_ABSPATH} and ${LIBSNDFILE_INC}
include(libsndfile.cmake)

# Krisp SDK libraries are applied to all targets
include(krisp.cmake)

# Libresample is required for Krisp SDK
include(libresample.cmake)

set(APPNAME_NC sample-nc)
set(APPNAME_SDK_EXT sample-sdk-ext)
set(DLLNAME krisp-dll)
set(APPNAME_DLL dll-test-app)
set(APPNAME_SDK_EXT_TEST test-sdk-ext)
#set(APPNAME_VAD sample-vad)

add_compile_definitions(KRISP_AUDIO_STATIC)
if (WIN32 AND MSVC)
	add_compile_definitions(_ITERATOR_DEBUG_LEVEL=0)
endif()

add_library(
	${DLLNAME} SHARED
	${ROOT_DIR}/src/sample-dll/dll-main.cpp
)

add_executable(
	${APPNAME_DLL}
	${ROOT_DIR}/src/sample-dll/dll-test.cpp
)

add_executable(
	${APPNAME_SDK_EXT_TEST}
	${ROOT_DIR}/src/sdk-extension/library_resources.cpp
	${ROOT_DIR}/src/sdk-extension/bvc_device_manager.cpp
	${ROOT_DIR}/src/sdk-extension/device_list.cpp
	${ROOT_DIR}/src/sdk-extension/model.cpp
	${ROOT_DIR}/src/sdk-extension/frame_cleaner.cpp
	${ROOT_DIR}/src/sdk-extension/frame_processor_factory.cpp
	${ROOT_DIR}/src/sdk-extension/mic_frame_cleaner_factory.cpp
	${ROOT_DIR}/src/sdk-extension-test/unit_test_device_list.cpp
)

add_executable(
	${APPNAME_NC} 
	${ROOT_DIR}/src/sample-nc/main.cpp
	${ROOT_DIR}/src/utils/sound_file.cpp
	${ROOT_DIR}/src/utils/argument_parser.cpp
)

add_executable(
	${APPNAME_SDK_EXT} 
	${ROOT_DIR}/src/sample-sdk-extension/main.cpp
	${ROOT_DIR}/src/sdk-extension/library_resources.cpp
	${ROOT_DIR}/src/utils/sound_file.cpp
	${ROOT_DIR}/src/utils/argument_parser.cpp
	${ROOT_DIR}/src/sdk-extension/bvc_device_manager.cpp
	${ROOT_DIR}/src/sdk-extension/device_list.cpp
	${ROOT_DIR}/src/sdk-extension/model.cpp
	${ROOT_DIR}/src/sdk-extension/helper.cpp
	${ROOT_DIR}/src/sdk-extension/frame_cleaner.cpp
	${ROOT_DIR}/src/sdk-extension/frame_processor_factory.cpp
	${ROOT_DIR}/src/sdk-extension/mic_frame_cleaner_factory.cpp
)

target_compile_definitions(
	${APPNAME_SDK_EXT_TEST}
	PRIVATE
	TEST_DATA_DIR="${ROOT_DIR}/src/sdk-extension-test/data"
)

target_compile_definitions(
	${APPNAME_SDK_EXT}
	PRIVATE
	TEST_DATA_DIR="${ROOT_DIR}/src/sdk-extension-test/data"
)

target_include_directories(
	${APPNAME_NC}
	PRIVATE
	${ROOT_DIR}/src/utils
	${LIBSNDFILE_INC}
	$ENV{KRISP_INC}
)

target_include_directories(
	${APPNAME_SDK_EXT}
	PRIVATE
	${ROOT_DIR}/src/utils
	${ROOT_DIR}/src/sdk-extension
	${LIBSNDFILE_INC}
	$ENV{KRISP_INC}
)

target_include_directories(
	${DLLNAME}
	PRIVATE
	$ENV{KRISP_INC}
)

target_include_directories(
	${APPNAME_DLL}
	PRIVATE
	$ENV{KRISP_INC}
)

target_include_directories(
	${APPNAME_SDK_EXT_TEST}
	PRIVATE
	./
	${ROOT_DIR}/googletest/googletest/include
	${ROOT_DIR}/src/sdk-extension
	$ENV{KRISP_INC}
)

target_link_libraries(
	${APPNAME_NC}
	${LIBSNDFILE_ABSPATH}
	${LIBKRISP_ABSPATH}
	${LIBRESAMPLE_ABSPATH}
)

target_link_libraries(
	${APPNAME_SDK_EXT}
	${LIBSNDFILE_ABSPATH}
	${LIBKRISP_ABSPATH}
	${LIBRESAMPLE_ABSPATH}
)

target_link_libraries(
	${DLLNAME}
	${LIBKRISP_ABSPATH}
	${LIBRESAMPLE_ABSPATH}
)

target_link_libraries(
	${APPNAME_DLL}
	${DLLNAME}
)

find_library(
	LIBGTEST_ABSPATH
	NAMES gtest
	PATHS ${ROOT_DIR}/build-googletest/lib
)

find_library(
	LIBGTEST_MAIN_ABSPATH
	NAMES gtest_main
	PATHS ${ROOT_DIR}/build-googletest/lib
)

target_link_libraries(
	${APPNAME_SDK_EXT_TEST}
	${LIBKRISP_ABSPATH}
	${LIBRESAMPLE_ABSPATH}
	${LIBGTEST_ABSPATH}
	${LIBGTEST_MAIN_ABSPATH}
)

# Blas libraries are applied to ${APPNAME_NC} target
# Blas libraries are applied to ${APPNAME_SDK_EXT} target
# Blas libraries are applied to ${APPNAME_SDK_EXT_TEST} target
# Blas libraries are applied to ${DLLNAME} target
include(blas.cmake)
