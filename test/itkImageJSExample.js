
if(process.argv.length < 4){
	console.error("itkImageJS example, set random pixels to the image.");
	console.error("Use mode: .nrrd or .nii files");
	console.error(process.argv[0], process.argv[1], " inputImage outputImage");
	return;
}

var {MedImgReader, FS} = require('../dist/index.js');

console.log(FS)
console.log(MedImgReader)
const medImgReader = new MedImgReader();
console.log(medImgReader)

var inputImage = process.argv[2];
var outputImage = process.argv[3];

console.log("Input image: ", inputImage);
console.log("Output image: ", outputImage);


// console.log("Reading image...");
// imagejs.SetFilename(inputImage);
// imagejs.ReadImage();

medImgReader.SetDirectory(inputImage);
medImgReader.ReadDICOMDirectory();


console.log("Spacing:", medImgReader.GetSpacing());
console.log("Origin:", medImgReader.GetOrigin());
console.log("Dimensions:", medImgReader.GetDimensions());
console.log("Direction:", medImgReader.GetDirection());
console.log("ComponentsPerPixel:", medImgReader.GetNumberOfComponentsPerPixel());
console.log("ImageBuffer:", medImgReader.GetImageBuffer());

console.log("Writing image...");
medImgReader.SetOutputFilename(outputImage);
medImgReader.WriteImage();