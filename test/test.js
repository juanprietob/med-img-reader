
const Lab = require('@hapi/lab');
const lab = exports.lab = Lab.script();
const path = require('path');
const MedImgReader = require('..');

lab.experiment("Test image-pad-resample", function(){

	lab.test('returns true when a 2D image with four components is read.', function(){

        const medImgReader = new MedImgReader();
		medImgReader.SetFilename("'/Users/prieto/web/us-famli-nn/1.2.276.0.26.1.1.1.2.2019.306.49060.7812131.180224000.nrrd'");
		medImgReader.ReadImage();

		var in_img = medImgReader.GetOutput();
		medImgReader.delete();

        return true;
    });

	lab.test('returns true when a 2D image with four components is read.', function(){

        const medImgReader = new MedImgReader();

		var inputImage = path.join(__dirname, 'gravitational.jpg')
		var outputImage = path.join(__dirname, 'gravitational_out.nrrd')

		console.log("Input image: ", inputImage);

		medImgReader.SetFilename(inputImage);
		medImgReader.ReadImage();

		var in_img = medImgReader.GetOutput();
		medImgReader.delete();
		console.log("Image:", in_img);
		
        const medImgReader2 = new MedImgReader();
		
		medImgReader2.SetInput(in_img);
		medImgReader2.SetFilename(outputImage);
		medImgReader2.WriteImage();

		console.log(medImgReader2.GetFilename());
		medImgReader2.GetStream(medImgReader2.GetFilename())
		.then((stream)=>{
			console.log(stream);
			medImgReader2.delete();
		})
		


        return true;
    });

	lab.test('returns true when a 2D image with four components is read.', function(){

        const medImgReader = new MedImgReader();

		var inputImage = path.join(__dirname, 'brain.png')
		var outputImage = path.join(__dirname, 'brain_out.nrrd')

		console.log("Input image: ", inputImage);

		medImgReader.SetFilename(inputImage);
		medImgReader.ReadImage();

		var in_img = medImgReader.GetOutput();
		console.log("Image:", in_img);
		medImgReader.delete();
		
        const medImgReader2 = new MedImgReader();
		
		medImgReader2.SetInput(in_img);
		medImgReader2.SetFilename(outputImage);
		medImgReader2.WriteImage();
		medImgReader2.delete();

        return true;
    });

    lab.test('returns true when a 3D image with 1 components is read.', function(){

        const medImgReader = new MedImgReader();

		var inputImage = path.join(__dirname, 't1.nrrd')
		var outputImage = path.join(__dirname, 't1_out.nii.gz')

		console.log("Input image: ", inputImage);

		medImgReader.SetFilename(inputImage);
		medImgReader.ReadImage();

		var in_img = medImgReader.GetOutput();
		console.log("Image:", in_img);
		medImgReader.delete();
		
        const medImgReader2 = new MedImgReader();
		
		medImgReader2.SetInput(in_img);
		medImgReader2.SetFilename(outputImage);
		medImgReader2.WriteImage();
		medImgReader2.delete();


        return true;
    });

    lab.test('returns true when a dcm image compressed is read.', function(){

        const medImgReader = new MedImgReader();

		var inputImage = path.join(__dirname, 'bmode.dcm')
		var outputImage = path.join(__dirname, 'bmode_out.nrrd')

		console.log("Input image: ", inputImage);

		medImgReader.SetFilename(inputImage);
		medImgReader.ReadImage();

		var in_img = medImgReader.GetOutput();
		console.log("Image:", in_img);
		medImgReader.delete();
		
        const medImgReader2 = new MedImgReader();
		
		medImgReader2.SetInput(in_img);
		medImgReader2.SetFilename(outputImage);
		medImgReader2.WriteImage();
		medImgReader2.delete();


        return true;
    });

    lab.test('returns true when a dcm series directory read.', function(){

        const medImgReader = new MedImgReader();
        //https://www.dicomlibrary.com?requestType=WADO&studyUID=1.2.826.0.1.3680043.8.1055.1.20111102150758591.92402465.76095170&manage=1b9baeb16d2aeba13bed71045df1bc65
		var inputDirectory = path.join(__dirname, 'series-000001')
		var outputImage = path.join(__dirname, 'series-000001.nrrd')

		console.log("Input directory: ", inputDirectory);

		medImgReader.SetDirectory(inputDirectory);
		medImgReader.ReadDICOMDirectory();

		var in_img = medImgReader.GetOutput();
		console.log("Image:", in_img);
		medImgReader.delete();
		
        const medImgReader2 = new MedImgReader();
		
		medImgReader2.SetInput(in_img);
		medImgReader2.SetFilename(outputImage);
		medImgReader2.WriteImage();
		medImgReader2.delete();

        return true;
    });
    

})