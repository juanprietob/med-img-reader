
#include "MedImgReader.h"

// Binding code
EMSCRIPTEN_BINDINGS(itk_image_j_s) {
  class_<MedImgReader>("MedImgReader")
    .constructor<>()
    .function("ReadImage", &MedImgReader::ReadImage)
    .function("ReadDICOMDirectory", &MedImgReader::ReadDICOMDirectory)
    .function("GetImageBuffer", &MedImgReader::GetImageBuffer)
    .function("GetDirection", &MedImgReader::GetDirection)
    .function("GetDimensions", &MedImgReader::GetDimensions)
    .function("GetNumberOfComponentsPerPixel", &MedImgReader::GetNumberOfComponentsPerPixel)
    .function("GetOrigin", &MedImgReader::GetOrigin)
    .function("GetSpacing", &MedImgReader::GetSpacing)
    .function("GetFilename", &MedImgReader::GetFilename)
    .function("SetFilename", &MedImgReader::SetFilename)
    .function("GetOutputFilename", &MedImgReader::GetOutputFilename)
    .function("SetOutputFilename", &MedImgReader::SetOutputFilename)
    .function("GetDirectory", &MedImgReader::GetDirectory)
    .function("SetDirectory", &MedImgReader::SetDirectory)
    .function("WriteImage", &MedImgReader::WriteImage)
    ;
}

MedImgReader::MedImgReader(){
  
}

MedImgReader::~MedImgReader(){
  
}


/*
* This function reads an image from the NODEFS or IDBS system and sets up the different attributes in MedImgReader
* If executing in the browser, you must save the image first using FS.write(filename, buffer).
* If executing inside NODE.js use mound directory with the image filename. 
*/

  void MedImgReader::ReadImage(){

    try{
      
      ImageFileReader::Pointer reader = ImageFileReader::New();
      string filename = this->GetFilename();
      cout<<"Reading "<<filename<<endl;
      reader->SetFileName(filename);
      reader->Update();
      InputImagePointerType image = reader->GetOutput();
      
      this->SetImage(image);
      this->Initialize();

    }catch(itk::ExceptionObject & err){
      cerr<<err<<endl;
    }
    
  }

  void MedImgReader::ReadDICOMDirectory(){

    try{

      using NamesGeneratorType = itk::GDCMSeriesFileNames;
      NamesGeneratorType::Pointer nameGenerator = NamesGeneratorType::New();

      string directory = this->GetDirectory();

      nameGenerator->SetUseSeriesDetails(true);
      nameGenerator->AddSeriesRestriction("0008|0021");
      nameGenerator->SetGlobalWarningDisplay(false);
      nameGenerator->SetDirectory(directory);


      using SeriesIdContainer = std::vector< std::string >;
      const SeriesIdContainer & seriesUID = nameGenerator->GetSeriesUIDs();
      auto seriesItr = seriesUID.begin();
      auto seriesEnd = seriesUID.end();

      if (seriesItr != seriesEnd){

        std::cout << "The directory: ";
        std::cout << directory << std::endl;
        std::cout << "Contains the following DICOM Series: ";
        std::cout << std::endl;

      }else{

        std::cout << "No DICOMs in: " << directory << std::endl;
        return;

      }

      while (seriesItr != seriesEnd){
        std::cout << seriesItr->c_str() << std::endl;
        ++seriesItr;
      }

      seriesItr = seriesUID.begin();
      while (seriesItr != seriesUID.end()){
        std::string seriesIdentifier;
        seriesIdentifier = seriesItr->c_str();
        seriesItr++;
      

        std::cout << "\nReading: ";
        std::cout << seriesIdentifier << std::endl;
        using FileNamesContainer = std::vector< std::string >;
        FileNamesContainer fileNames = nameGenerator->GetFileNames(seriesIdentifier);

        using ReaderType = itk::ImageSeriesReader< InputImageType >;
        ReaderType::Pointer reader = ReaderType::New();
        using ImageIOType = itk::GDCMImageIO;
        ImageIOType::Pointer dicomIO = ImageIOType::New();
        reader->SetImageIO(dicomIO);
        reader->SetFileNames(fileNames);
        reader->Update();
        this->SetImage(reader->GetOutput());
      }
      
      this->Initialize();

    }catch(itk::ExceptionObject & err){
      cerr<<err<<endl;
    }
    
  }

  /*
  * After reading the image, it sets up different attributes
  */
  void MedImgReader::Initialize(){

    InputImagePointerType img = this->GetImage();

    OrientImageFilterPointerType orienter = OrientImageFilterType::New();
    orienter->UseImageDirectionOn();
    orienter->SetDesiredCoordinateOrientation(itk::SpatialOrientation::ITK_COORDINATE_ORIENTATION_RAS);
    orienter->SetInput(img);
    orienter->Update();
    this->SetImage(orienter->GetOutput());

    img = this->GetImage();

    SizeType size = img->GetLargestPossibleRegion().GetSize();
    m_Size[0] = size[0];
    m_Size[1] = size[1];
    m_Size[2] = size[2];

    SpacingType spacing = img->GetSpacing();
    m_Spacing[0] = spacing[0];
    m_Spacing[1] = spacing[1];
    m_Spacing[2] = spacing[2];
    
    DirectionType direction = img->GetDirection();

    for(int i = 0; i < dimension*dimension; i++){
      m_Direction[i] = direction[i/dimension][i%dimension];
    }

    PointType origin = img->GetOrigin();

    m_Origin[0] = origin[0];
    m_Origin[1] = origin[1];
    m_Origin[2] = origin[2];
    
  }

  /*
  * Write the image to to the file system. 
  */
  void MedImgReader::WriteImage(){
    try{
    
      ImageFileWriter::Pointer writer = ImageFileWriter::New();
      string filename = this->GetOutputFilename();
      writer->SetFileName(filename);
      writer->SetInput(this->GetImage());
      writer->Update();
    }catch(itk::ExceptionObject & err){
      cerr<<err<<endl;
    }

  }