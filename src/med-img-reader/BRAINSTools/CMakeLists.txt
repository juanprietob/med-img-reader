project(BRAINSTools)


set(BRAINSTools_DIR "" CACHE PATH "Directory of BRAINSTools")

if(BRAINSTools_DIR)

	set( ITK_INCLUSION_MODULES
	    # Everything needs ITKCommon
	    ITKCommon
	    # Common depends on these modules
	    ITKVNLInstantiation
	    ITKKWSys
	    ITKDoubleConversion
	    ITKVNLInstantiation
	    ITKVNL
	    # IO Components
	    ITKIOImageBase
	    ITKIOBMP
	    ITKIOBioRad
	    ITKIOGDCM
	    ITKIOGIPL
	    ITKIOJPEG
	    ITKIOLSM
	    ITKIOMeta
	    ITKIONIFTI
	    ITKIONRRD
	    ITKIOPNG
	    ITKIOStimulate
	    ITKIOTIFF
	    ITKIOTransformInsightLegacy
	    #ITKIOVTK
	    ITKIOSpatialObjects
	    ITKIOTransformBase
	    ITKIOHDF5
	    ITKIOTransformMatlab
	    ITKIOTransformHDF5
	    ITKIOGE
	    MGHIO
	    # other modules specific to the current directory
	    ITKImageLabel
		ITKDistanceMap
		ITKLabelVoting
		ITKConnectedComponents
		ITKMathematicalMorphology
		ITKBinaryMathematicalMorphology
		ITKThresholding
		ITKImageCompare
		ITKSmoothing
		ITKRegistrationCommon
		ITKRegistrationMethodsv4
		ITKMetricsv4
		ITKOptimizersv4
		ITKRegionGrowing
		ITKNrrdIO

		ITKImageIntensity
		ITKImageStatistics
		ITKTestKernel
		ITKIODCMTK
		ITKIORAW
		ITKDCMTK
	)
	list(REMOVE_DUPLICATES ITK_INCLUSION_MODULES)
	find_package(ITK COMPONENTS ${ITK_INCLUSION_MODULES} REQUIRED)
	include(${ITK_USE_FILE})

	find_package(SlicerExecutionModel)
	include(${SlicerExecutionModel_USE_FILE})

	set(BRAINSCommonLib_DIR ${BRAINSTools_DIR}/BRAINSCommonLib/)
	set(DWIConvert_DIR ${BRAINSTools_DIR}/DWIConvert)

	set(BRAINSTools_VERSION_MAJOR 4)
	set(BRAINSTools_VERSION_MINOR 7)
	set(BRAINSTools_VERSION_PATCH 1)
	set(BRAINSTools_VERSION_TWEAK 0)
	configure_file("${BRAINSCommonLib_DIR}/BRAINSToolsVersionConfig.h.in" "BRAINSToolsVersionConfig.h" @ONLY IMMEDIATE)

	configure_file(
	  ${BRAINSCommonLib_DIR}/BRAINSCommonLib.h.in
	  ${CMAKE_CURRENT_BINARY_DIR}/BRAINSCommonLib.h
	  )

	include_directories(
	  ${BRAINSCommonLib_DIR}
	  ${DWIConvert_DIR}
	  ${CMAKE_CURRENT_BINARY_DIR}
	  )

	set(BRAINSCommonLib_SRCS
	  ${BRAINSCommonLib_DIR}/BRAINSCommonLib.cxx
	  ${BRAINSCommonLib_DIR}/GenericTransformImage.cxx
	  ${BRAINSCommonLib_DIR}/BRAINSFitHelper.cxx
	  ${BRAINSCommonLib_DIR}/BRAINSComputeLabels.cxx
	  ${BRAINSCommonLib_DIR}/Slicer3LandmarkWeightIO.cxx
	  ${BRAINSCommonLib_DIR}/Slicer3LandmarkIO.cxx
	  ${BRAINSCommonLib_DIR}/itkOrthogonalize3DRotationMatrix.cxx
	  ${BRAINSCommonLib_DIR}/BRAINSThreadControl.cxx
	  ${BRAINSCommonLib_DIR}/ExtractSingleLargestRegion.cxx
	  ${BRAINSCommonLib_DIR}/BRAINSToolsVersion.cxx
	  ${BRAINSCommonLib_DIR}/DWIMetaDataDictionaryValidator.cxx
	)

	## Always build BRAINSCommonLib as static
	add_library(BRAINSCommonLib STATIC ${BRAINSCommonLib_SRCS})
	target_link_libraries(BRAINSCommonLib ${ITK_LIBRARIES})

	# SEMMacroBuildCLI(
	# 	NAME DWIConvert
	# 	DISABLE_DEFAULT_CXX_FILE
	# 	EXECUTABLE_ONLY
	# 	CLI_XML_FILE ${DWIConvert_DIR}/DWIConvert.xml
	# 	ADDITIONAL_SRCS ${DWIConvert_DIR}/DWIConvert.cxx
	# 	TARGET_LIBRARIES BRAINSCommonLib ${ITK_LIBRARIES}
	# )
	add_executable(DWIConvert ${DWIConvert_DIR}/DWIConvert.cxx)
	target_link_libraries(DWIConvert BRAINSCommonLib)

endif()

# SET(EXE_NAME itkImageJS)
# SET(${EXE_NAME}_LIBRARIES  ${ITK_LIBRARIES})
# SET(${EXE_NAME}_SOURCE ${EXE_NAME}.cxx)
# add_executable(${EXE_NAME}   ${${EXE_NAME}_SOURCE})
# target_link_libraries(${EXE_NAME} ${${EXE_NAME}_LIBRARIES})