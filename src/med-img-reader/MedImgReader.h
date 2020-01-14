
#ifndef _MedImgReader_
#define _MedImgReader_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#include <bind.h>
#include <val.h>

#include <itkVectorImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageIOFactory.h>
#include <itkDataObject.h>
#include <itkSmartPointer.h>

#include <itkGDCMSeriesFileNames.h>
#include <itkImageSeriesReader.h>
#include <itkGDCMImageIO.h>
#include <itksys/SystemTools.hxx>


using namespace std;
using namespace emscripten;

EM_JS(bool, environmentIsNode, (), {
  if (ENVIRONMENT_IS_NODE) {
    return true;
  }
  return false;
});

EM_JS(const char*, resolvePath, (const char* filename), {
  var path = require('path');
  var resolvedfilename = path.resolve(UTF8ToString(filename));
  var lengthBytes = lengthBytesUTF8(resolvedfilename)+1;
  var stringOnWasmHeap = _malloc(lengthBytes);
  stringToUTF8(resolvedfilename, stringOnWasmHeap, lengthBytes);
  return stringOnWasmHeap;
});

EM_JS(const char*, baseName, (const char* filename), {
  var path = require('path');
  var dirname = path.dirname(UTF8ToString(filename));
  var lengthBytes = lengthBytesUTF8(dirname)+1;
  var stringOnWasmHeap = _malloc(lengthBytes);
  stringToUTF8(dirname, stringOnWasmHeap, lengthBytes);
  return stringOnWasmHeap;
});

EM_JS(const char*, dirName, (const char* filename), {
  var path = require('path');
  var dirname = path.dirname(UTF8ToString(filename));
  var lengthBytes = lengthBytesUTF8(dirname)+1;
  var stringOnWasmHeap = _malloc(lengthBytes);
  stringToUTF8(dirname, stringOnWasmHeap, lengthBytes);
  return stringOnWasmHeap;
});

EM_JS(void, mountDirectory, (const char* directory), {

  var path = require('path');
  var fulldir = path.resolve(UTF8ToString(directory));

  var currentdir = '/';
  fulldir.split('/').forEach(function(dir){
    currentdir += '/' + dir;
    currentdir = path.normalize(currentdir);
    try{
      FS.mkdir(currentdir);
    }catch(e){

    }
  });
  try{
    FS.mount(NODEFS, { root: fulldir }, fulldir);
  }catch(e){
    
  }
});

struct MedImgReaderBase {
public:

  using DataObjectType = typename itk::DataObject;
  typedef itk::SmartPointer<DataObjectType> SmartDataObjectType;

  void ReadImage();

  void ReadDICOMDirectory();

  void WriteImage();

  template <unsigned int VDimension>
  void WriteImageDimension();

  template <class TImage>
  void WriteImageTyped();

  string GetFilename(){
    return m_Filename;
  }

  void SetFilename(string filename){
    if(environmentIsNode()){
      mountDirectory(resolvePath(dirName(filename.c_str())));
      m_Filename = string(resolvePath(filename.c_str()));
    }else{
      m_Filename = filename;  
    }
  }

  void SetDirectory(string directory){
    if(environmentIsNode()){
      mountDirectory(resolvePath(directory.c_str()));
      m_Directory = string(resolvePath(directory.c_str()));
    }else{
      m_Directory = directory;  
    }
  }

  string GetDirectory(){
    return m_Directory;
  }

  val GetOutput();
  template <unsigned int VDimension>
  val GetOutputDimensionType();

  template <typename ImageType>
  val GetOutputImageType(string componentType);

  void SetInput(val const & image);

  template<unsigned int VDimension>
  void SetInputDimensionTyped(val const & image);

  template<typename PixelType, unsigned int VDimension>
  void SetInputTyped(val const & image);

  template <typename ImagePointerType>
  void SetITKImage(ImagePointerType image){ m_Image = image; }

  void SetImageDimension(const int dim){
    m_Dimension = dim;
  }

  int GetImageDimension(){
    return m_Dimension;
  }

  void SetComponentType(const itk::ImageIOBase::IOComponentType componentType){
    m_ComponentType = componentType;
  }

  itk::ImageIOBase::IOComponentType GetComponentType(){
    return m_ComponentType;
  }

  void SetPixelType(const itk::ImageIOBase::IOPixelType pixelType){
    m_PixelType = pixelType;
  }

  itk::ImageIOBase::IOPixelType GetPixelType(){
    return m_PixelType;
  }

  template <unsigned int VDimension>
  int ReadVectorImage(const char * inputFileName);

  template <class TImage>
  int ReadImageT(const char * fileName);

  virtual void WriteFile() = 0;
  virtual void GetStream() = 0;
  virtual void CloseStream() = 0;
  virtual void MakeDirectory() = 0;

private:
  string m_Filename;
  string m_Directory;
  SmartDataObjectType m_Image;
  int m_Dimension;
  itk::ImageIOBase::IOComponentType m_ComponentType;
  itk::ImageIOBase::IOPixelType m_PixelType;
  
  
};

#endif