
#include "MedImgReader.h"

struct MedImgReader : public wrapper<MedImgReaderBase> {
  EMSCRIPTEN_WRAPPER(MedImgReader);

  void WriteFile(){
    call<void>("WriteFile");
  }

  void GetStream(){
    call<void>("GetStream");
  }

  void CloseStream(){
    call<void>("CloseStream");
  }

  void MakeDirectory(){
    call<void>("MakeDirectory");
  }
};

  

// Binding code
EMSCRIPTEN_BINDINGS(med_img_reader) {
  class_<MedImgReaderBase>("MedImgReaderBase")
    .function("GetFilename", &MedImgReaderBase::GetFilename)
    .function("SetFilename", &MedImgReaderBase::SetFilename)
    .function("ReadImage", &MedImgReaderBase::ReadImage)    

    .function("GetDirectory", &MedImgReaderBase::GetDirectory)
    .function("SetDirectory", &MedImgReaderBase::SetDirectory)
    .function("ReadDICOMDirectory", &MedImgReaderBase::ReadDICOMDirectory)

    .function("GetOutput", &MedImgReaderBase::GetOutput)

    .function("SetInput", &MedImgReaderBase::SetInput)
    .function("WriteImage", &MedImgReaderBase::WriteImage)

    .function("WriteFile", &MedImgReaderBase::WriteFile, pure_virtual())
    .function("GetStream", &MedImgReaderBase::GetStream, pure_virtual())
    .function("CloseStream", &MedImgReaderBase::CloseStream, pure_virtual())
    .function("MakeDirectory", &MedImgReaderBase::MakeDirectory, pure_virtual())
    .allow_subclass<MedImgReader>("MedImgReader")
    ;
}

void MedImgReaderBase::SetInput(val const & image){

  if(image["imageType"]["dimension"].as<unsigned>() == 1){
    this->SetInputDimensionTyped<1>(image);
    this->SetImageDimension(1);
  }else if(image["imageType"]["dimension"].as<unsigned>() == 2){
    this->SetInputDimensionTyped<2>(image);
    this->SetImageDimension(2);
  }else if(image["imageType"]["dimension"].as<unsigned>() == 3){
    this->SetInputDimensionTyped<3>(image);  
    this->SetImageDimension(3);
  }else{
    this->SetInputDimensionTyped<4>(image);
    this->SetImageDimension(4);
  }
}

template <unsigned int VDimension>
void MedImgReaderBase::SetInputDimensionTyped(val const & image){
  
  string componentType = image["imageType"]["componentType"].as<string>();
  const unsigned int dimension = VDimension;

  if(componentType.compare("int8_t") == 0){
    this->SetInputTyped<char, VDimension>(image);
    this->SetComponentType(itk::ImageIOBase::CHAR);
  }else if(componentType.compare("uint8_t") == 0){
    this->SetInputTyped<unsigned char, VDimension>(image);
    this->SetComponentType(itk::ImageIOBase::UCHAR);
  }else if(componentType.compare("int16_t") == 0){
    this->SetInputTyped<short, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::SHORT);
  }else if(componentType.compare("uint16_t") == 0){
    this->SetInputTyped<unsigned short, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::USHORT);
  }else if(componentType.compare("int32_t") == 0){
    this->SetInputTyped<int, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::INT);
  }else if(componentType.compare("uint32_t") == 0){
    this->SetInputTyped<unsigned int, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::UINT);
  }else if(componentType.compare("int64_t") == 0){
    this->SetInputTyped<long, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::LONG);
  }else if(componentType.compare("uint64_t") == 0){
    this->SetInputTyped<unsigned long, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::ULONG);
  }else if(componentType.compare("float") == 0){
    this->SetInputTyped<float, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::FLOAT);
  }else{
    this->SetInputTyped<double, dimension>(image);
    this->SetComponentType(itk::ImageIOBase::DOUBLE);
  }
}

template<typename PixelType, unsigned int VDimension>
void MedImgReaderBase::SetInputTyped(val const & image){

  unsigned components = image["imageType"]["components"].as<unsigned>();

  typedef itk::VectorImage< PixelType, VDimension > ImageType;

  typename ImageType::PixelType zero(components);
  zero.Fill(0);

  const int dimension = VDimension;

  typename ImageType::SizeType size;
  size.Fill(1);
  
  for(unsigned i = 0; i < dimension; ++i) {
    size[i] = image["size"][i].as<int>();
  }

  typename ImageType::RegionType region;
  region.SetSize(size);

  typename ImageType::PointType origin;

  for(unsigned i = 0; i < dimension; ++i) {
    origin[i] = image["origin"][i].as<double>();
  }
  
  typename ImageType::SpacingType spacing;
  spacing.Fill(1);

  for(unsigned i = 0; i < dimension; ++i) {
    spacing[i] = image["spacing"][i].as<double>();
  }

  typename ImageType::DirectionType direction;

  for(unsigned i = 0; i < dimension*dimension; ++i) {
    direction.GetVnlMatrix().data_block()[i] = image["direction"]["data"][i].as<double>();
  }


  typename ImageType::Pointer itk_image = ImageType::New();
  itk_image->SetNumberOfComponentsPerPixel(components);
  itk_image->SetRegions(region);
  itk_image->SetOrigin(origin);
  itk_image->SetSpacing(spacing);
  itk_image->SetDirection(direction);
  itk_image->Allocate();
  
  typename ImageType::InternalPixelType* buffer = itk_image->GetBufferPointer();

  unsigned numberofpixels = image["data"]["length"].as<unsigned>();

  for(unsigned i = 0; i < numberofpixels; ++i) {
    buffer[i] = image["data"][i].as<PixelType>();
  }

  this->SetITKImage(itk_image);

}

/*
* This function reads an image from the NODEFS or IDBS system and sets up the different attributes in MedImgReaderBase
* If executing in the browser, you must save the image first using FS.write(filename, buffer).
* If executing inside NODE.js use mound directory with the image filename. 
*/

void MedImgReaderBase::ReadImage(){

  string filename = this->GetFilename();

  const char * inputFileName = filename.c_str();

  itk::ImageIOBase::Pointer imageIO = itk::ImageIOFactory::CreateImageIO(inputFileName, itk::ImageIOFactory::FileModeType::ReadMode);

  imageIO->SetFileName(inputFileName);
  imageIO->ReadImageInformation();

  using IOPixelType = itk::ImageIOBase::IOPixelType;
  const IOPixelType pixelType = imageIO->GetPixelType();  

  using IOComponentType = itk::ImageIOBase::IOComponentType;
  const IOComponentType componentType = imageIO->GetComponentType();

  const unsigned int imageDimension = imageIO->GetNumberOfDimensions();

  this->SetComponentType(componentType);
  this->SetPixelType(pixelType);
  
  if (imageDimension == 2)
  {
    this->ReadVectorImage<2>(inputFileName);
  }
  else if (imageDimension == 3)
  {
    this->ReadVectorImage<3>(inputFileName);
  }
  else if (imageDimension == 4)
  {
    this->ReadVectorImage<4>(inputFileName);
  }else{
    throw "mmm... maybe save the image as 3D but with multiple components? I have no idea what to do with this";
  }
  
}

void MedImgReaderBase::ReadDICOMDirectory(){

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
      
      using ImageType = itk::VectorImage< short, 3>;

      using ReaderType = itk::ImageSeriesReader< ImageType >;
      ReaderType::Pointer reader = ReaderType::New();
      using ImageIOType = itk::GDCMImageIO;
      ImageIOType::Pointer dicomIO = ImageIOType::New();
      reader->SetImageIO(dicomIO);
      reader->SetFileNames(fileNames);
      reader->Update();
      
      using ImagePointerType = typename ImageType::Pointer;
      ImagePointerType image = reader->GetOutput();

      this->SetITKImage(image);
      this->SetImageDimension(image->GetImageDimension());
      this->SetComponentType(itk::ImageIOBase::SHORT);
      this->SetPixelType(itk::ImageIOBase::IOPixelType::SCALAR);
      
    }

  }catch(itk::ExceptionObject & err){
    cerr<<err<<endl;
  }
  
}

/*
* Write the image to to the file system. 
*/
void MedImgReaderBase::WriteImage(){
  try{
    if(this->GetImageDimension() == 1){
      this->WriteImageDimension<1>();
    }else if(this->GetImageDimension() == 2){
      return this->WriteImageDimension<2>();
    }else if(this->GetImageDimension() == 3){
      return this->WriteImageDimension<3>();
    }else{
      return this->WriteImageDimension<4>();
    }
  }catch(itk::ExceptionObject & err){
    cerr<<err<<endl;
  }

}

template <unsigned int VDimension>
void MedImgReaderBase::WriteImageDimension(){
  switch (this->GetComponentType())
  {
    default:
    case itk::ImageIOBase::UCHAR:
    {
      using PixelType = unsigned char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();

      break;
    }

    case itk::ImageIOBase::CHAR:
    {
      using PixelType = char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      using PixelType = unsigned short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      using PixelType = short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      using PixelType = unsigned int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::INT:
    {
      using PixelType = int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::ULONG:
    {
      using PixelType = unsigned long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::LONG:
    {
      using PixelType = long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      using PixelType = float;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {
      using PixelType = double;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      this->WriteImageTyped<ImageType>();
      break;
    }
  }
}

template <class TImage>
void MedImgReaderBase::WriteImageTyped(){

  using ImageType = TImage;
  using ImageFileWriter = itk::ImageFileWriter<ImageType>;
  typename ImageFileWriter::Pointer writer = ImageFileWriter::New();
  string filename = this->GetFilename();
  writer->UseCompressionOn();
  writer->SetFileName(filename);
  ImageType* itk_image = static_cast<ImageType*>(m_Image);
  writer->SetInput(itk_image);
  writer->Update();
}

val MedImgReaderBase::GetOutput(){
  if(this->GetImageDimension() == 2){
    return this->GetOutputDimensionType<2>();
  }else if(this->GetImageDimension() == 3){
    return this->GetOutputDimensionType<3>();
  }else{
    return this->GetOutputDimensionType<4>();
  }
}

template <unsigned int VDimension>
val MedImgReaderBase::GetOutputDimensionType(){
  
  
  switch (this->GetComponentType())
  {
    default:
    case itk::ImageIOBase::UCHAR:
    {
      using PixelType = unsigned char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("uint8_t");

      break;
    }

    case itk::ImageIOBase::CHAR:
    {
      using PixelType = char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("int8_t");
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      using PixelType = unsigned short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("uint16_t");
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      using PixelType = short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("int16_t");
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      using PixelType = unsigned int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("uint32_t");
      break;
    }

    case itk::ImageIOBase::INT:
    {
      using PixelType = int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("int32_t");
      break;
    }

    case itk::ImageIOBase::ULONG:
    {
      using PixelType = unsigned long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("uint64_t");
      break;
    }

    case itk::ImageIOBase::LONG:
    {
      using PixelType = long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("int64_t");
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      using PixelType = float;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("float");
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {
      using PixelType = double;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      return this->GetOutputImageType<ImageType>("double");
      break;
    }
  }
}

template <typename ImageType>
val MedImgReaderBase::GetOutputImageType(string componentType)
{
  // image = {
  //   imageType: {
  //     dimension: 2,
  //     componentType: 'uint16_t',
  //     pixelType: 1,
  //     components: 1
  //   },
  //   name: 'Image',
  //   origin: [ 0, 0 ],
  //   spacing: [ 0.148489, 0.148489 ],
  //   direction: { rows: 2, columns: 2, data: [ 1, 0, 0, 1 ] },
  //   size: [ 1136, 852 ],
  //   data: Uint16Array []
  // }
  ImageType* itk_image = static_cast<ImageType*>(m_Image);

  val origin = val::array();
  val spacing = val::array();
  val size = val::array();
  val direction = val::array();

  typename ImageType::SpacingType i_spacing = itk_image->GetSpacing();
  typename ImageType::PointType i_origin = itk_image->GetOrigin();
  typename ImageType::SizeType i_size = itk_image->GetLargestPossibleRegion().GetSize();
  typename ImageType::DirectionType::InternalMatrixType i_direction = itk_image->GetDirection().GetVnlMatrix();

  int i_dimension = itk_image->GetImageDimension();

  for(unsigned i = 0; i < i_dimension; i++){
    origin.call<void>("push", i_origin[i]);
    spacing.call<void>("push", i_spacing[i]);
    size.call<void>("push", i_size[i]);
  }

  for(unsigned i = 0; i < i_direction.size(); i++){
    direction.call<void>("push", i_direction.data_block()[i]);
  }

  val imageType = val::object();
  imageType.set("dimension", i_dimension);
  imageType.set("components", (int)itk_image->GetNumberOfComponentsPerPixel());
  imageType.set("componentType", val(componentType));
  imageType.set("pixelType", (int)this->GetPixelType());

  val matrixType = val::object();
  matrixType.set("rows", i_direction.rows());
  matrixType.set("columns", i_direction.columns());
  matrixType.set("data", direction);

  val image = val::object();
  image.set("imageType", imageType);
  image.set("name", val("Image"));
  image.set("origin", origin);
  image.set("spacing", spacing);
  image.set("direction", matrixType);
  image.set("size", size);
  image.set("data", val(typed_memory_view((int)itk_image->GetPixelContainer()->Size(),itk_image->GetBufferPointer())));

  return image;
}

template <class TImage>
int
MedImgReaderBase::ReadImageT(const char * fileName){
  using ImageType = TImage;
  using ImageReaderType = itk::ImageFileReader<ImageType>;

  typename ImageReaderType::Pointer reader = ImageReaderType::New();
  reader->SetFileName(fileName);

  try
  {
    reader->Update();
  }
  catch (itk::ExceptionObject & e)
  {
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
  }

  using ImagePointerType = typename ImageType::Pointer;
  ImagePointerType image = reader->GetOutput();

  this->SetITKImage(image);
  this->SetImageDimension(image->GetImageDimension());

  return EXIT_SUCCESS;
}

template <unsigned int VDimension>
int
MedImgReaderBase::ReadVectorImage(const char * inputFileName)
{
  switch (this->GetComponentType())
  {
    default:
    case itk::ImageIOBase::UNKNOWNCOMPONENTTYPE:
      std::cerr << "Unknown and unsupported component type!" << std::endl;
      return EXIT_FAILURE;

    case itk::ImageIOBase::UCHAR:
    {
      using PixelType = unsigned char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }

      break;
    }

    case itk::ImageIOBase::CHAR:
    {
      using PixelType = char;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::USHORT:
    {
      using PixelType = unsigned short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::SHORT:
    {
      using PixelType = short;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::UINT:
    {
      using PixelType = unsigned int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::INT:
    {
      using PixelType = int;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::ULONG:
    {
      using PixelType = unsigned long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::LONG:
    {
      using PixelType = long;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::FLOAT:
    {
      using PixelType = float;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }

    case itk::ImageIOBase::DOUBLE:
    {
      using PixelType = double;
      using ImageType = itk::VectorImage<PixelType, VDimension>;

      typename ImageType::Pointer image = ImageType::New();

      if (this->ReadImageT<ImageType>(inputFileName) == EXIT_FAILURE)
      {
        return EXIT_FAILURE;
      }
      break;
    }
  }
  return EXIT_SUCCESS;
}
