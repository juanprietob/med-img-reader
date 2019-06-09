
#ifndef _MedImgReader_
#define _MedImgReader_

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <emscripten.h>
#include <bind.h>
#include <val.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkOrientImageFilter.h>

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

EM_JS(void, mountDirectory, (const char* filename), {
  var path = require('path');

  var filename = path.resolve(UTF8ToString(filename));
  var dirname = path.dirname(filename);
  var fulldir = path.resolve(dirname);

  var currentdir = '';
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
    console.error(e);
  }
});

class MedImgReader {
public:

  static const int dimension = 3;
  typedef short PixelType;
  typedef itk::Image< PixelType, dimension > InputImageType;
  typedef typename InputImageType::Pointer InputImagePointerType;
  typedef typename InputImageType::IndexType InputImageIndexType;
  typedef typename InputImageType::SpacingType SpacingType;
  typedef typename InputImageType::PointType PointType;
  typedef typename InputImageType::RegionType RegionType;
  typedef typename InputImageType::SizeType SizeType;
  typedef typename InputImageType::DirectionType DirectionType;
  
  typedef itk::ImageFileReader< InputImageType > ImageFileReader;
  typedef itk::ImageFileWriter< InputImageType > ImageFileWriter;

  typedef itk::OrientImageFilter< InputImageType, InputImageType > OrientImageFilterType;
  typedef typename OrientImageFilterType::Pointer OrientImageFilterPointerType;

  MedImgReader();
  ~MedImgReader();

  void Initialize();

  void ReadImage();

  void ReadDICOMDirectory();

  void WriteImage();

  val GetImageBuffer(){
    return val(typed_memory_view((int)this->GetImage()->GetPixelContainer()->Size(),this->GetImage()->GetBufferPointer()));
  }

  val GetSpacing(){
    return val(typed_memory_view(3, m_Spacing));
  }

  val GetOrigin(){
    return val(typed_memory_view(3, m_Origin));
  }

  int GetNumberOfComponentsPerPixel(){
    return this->GetImage()->GetNumberOfComponentsPerPixel();
  }

  val GetDimensions(){
    return val(typed_memory_view(3, m_Size));
  }

  val GetDirection(){
    return val(typed_memory_view(9, m_Direction));
  }

  string GetFilename(){
    return m_Filename;
  }

  void SetFilename(string filename){
    if(environmentIsNode()){
      mountDirectory(filename.c_str());
      m_Filename = string(resolvePath(filename.c_str()));
    }else{
      m_Filename = filename;  
    }
  }

  string GetOutputFilename(){
    return m_OutputFilename;
  }

  void SetOutputFilename(string filename){
    if(environmentIsNode()){
      mountDirectory(filename.c_str());
      m_OutputFilename = string(resolvePath(filename.c_str()));
    }else{
      m_OutputFilename = filename;  
    }
  }

  void SetDirectory(string directory){
    if(environmentIsNode()){
      mountDirectory(directory.c_str());
      m_Directory = string(resolvePath(directory.c_str()));
    }else{
      m_Directory = directory;  
    }
  }

  string GetDirectory(){
    return m_Directory;
  }

  InputImagePointerType GetImage() const { return m_Image; }
  void SetImage(InputImagePointerType image){ m_Image = image; }

private:
  string m_Filename;
  string m_OutputFilename;
  string m_Directory;
  InputImagePointerType m_Image;
  
  double m_Spacing[3];
  double m_Origin[3];
  double m_Direction[9];
  int m_Size[3];
  
  
};

#endif