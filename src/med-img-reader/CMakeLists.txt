

cmake_minimum_required(VERSION 3.3)
include(ExternalProject)

SET(FLAGS "--bind -Wno-warn-absolute-paths --memory-init-file 0 -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 -s EXPORT_ES6=1 -s MODULARIZE=1 -s EXPORT_NAME=\"'MedImgReader'\" -s 'EXTRA_EXPORTED_RUNTIME_METHODS=[\"FS\"]'" )

SET (CMAKE_CXX_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)
SET (CMAKE_C_FLAGS ${FLAGS} CACHE STRING "compile flags" FORCE)

set(ITK_IO_MODULES_USED 
		ITKIONIFTI
		ITKIONRRD
		MGHIO
		ITKIOGDCM
		ITKImageGrid
	)
find_package(ITK COMPONENTS
  ITKCommon
  ITKIOImageBase
  ITKImageFunction
  ${ITK_IO_MODULES_USED}
)
IF(${ITK_FOUND})
	include(${ITK_USE_FILE})
ENDIF(${ITK_FOUND})


INCLUDE_DIRECTORIES($ENV{EMSCRIPTEN}/system/include/emscripten/)

SET(EXE_NAME MedImgReader)
SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})
SET(${EXE_NAME}_SOURCE MedImgReader.cxx)
add_executable(${EXE_NAME} ${${EXE_NAME}_SOURCE})
target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})

# subdirs(BRAINSTools)