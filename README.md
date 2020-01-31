# med-img-reader

Read or write a 3D or 2D image with one or multiple components in a variety of formats. 
DICOM, NIFTI, NRRD, JPG, PNG, MHD
[Viewer example](https://hpc.medimg-ai.com)

## Installation

----
	npm install med-img-reader
----

## Usage example

### In NODE environment

#### Read a single image file

---
	const MedImgReader = require('med-img-reader');

	//Image with one or multiple components in any format
	var inputImage = '/path/to/input{.nrrd,.nii.gz,.jpg,.png,.dcm}'
	var outputImage = '/path/to/output{.nrrd,.nii.gz,.jpg,.png,.dcm}'


	const medImgReader = new MedImgReader();
	medImgReader.SetFilename(inputImage);
	medImgReader.ReadImage();
	var image = medImgReader.GetOutput();
	console.log("Image:", image);
	

    const medImgWriter = new MedImgReader();
	
	medImgWriter.SetInput(image);
	medImgWriter.SetFilename(outputImage);
	medImgWriter.WriteImage();
---

This is an example of an object return by the reader, which is compatible with [itk.js](https://insightsoftwareconsortium.github.io/itk-js/docs/index.html)

---
	image = {
	    imageType: {
	      dimension: 2,
	      componentType: 'uint16_t',
	      pixelType: 1,
	      components: 1
	    },
	    name: 'Image',
	    origin: [ 0, 0 ],
	    spacing: [ 0.148489, 0.148489 ],
	    direction: { rows: 2, columns: 2, data: [ 1, 0, 0, 1 ] },
	    size: [ 256, 256 ],
	    data: Uint16Array [...]
	}
---

### Convert the image to a Tensor from tensorflow [tfjs](https://www.tensorflow.org/js/)

---
	const tf = require('@tensorflow/tfjs-node');//Or tfjs in browser or tfjs-node-gpu if in linux

	tf.tensor(
		Float32Array.from(image.data), 
		[...[...image.size].reverse(), image.imageType.components]
	));
---


#### Read a DICOM series

---
        
	var inputDirectory = '/path/to/series/directory'
	var outputImage = 'out.nrrd';

	const medImgReader = new MedImgReader();
	medImgReader.SetDirectory(inputDirectory);
	medImgReader.ReadDICOMDirectory();

	var image = medImgReader.GetOutput();

	const medImgWriter = new MedImgReader();
	medImgWriter.SetInput(inputImage);
	medImgWriter.SetFilename(outputImage);
	medImgWriter.WriteImage();
---

### In browser environment 

#### React component

If you are going to use this in the browser, the build time will be long so be patient.
This library is compiled using [emscripten](https://emscripten.org/) and it bundles a file system with the [FS library](https://emscripten.org/docs/api_reference/Filesystem-API.html).

Here is an example for a React component:

---
	import React, { Component } from 'react'

	const axios = require('axios');
	const MedImgReader = require('med-img-reader');

	export default class ExampleComponent extends Component {

	  constructor(){
	    super();

	    this.state = {
	      itkImage: {}
	    }

	    const self = this;
	    var medImgReader = new MedImgReader();

	    var filename = '/brain.png';

	    axios({
	      method: 'get',
	      url: filename,
	      responseType: 'blob'
	    })
	    .then(function(brain){
	      var blob = brain.data;
	      return blob.arrayBuffer()
	      .then((arr)=>{
	        medImgReader.WriteFile(filename, arr);//We add the file to the FS filesystem
	        medImgReader.SetFilename(filename);//Set the file name 
	        medImgReader.ReadImage();
	        return medImgReader.GetOutput();
	      })
	      
	    })
	    .then((itkImage)=>{
	      self.setState({...self.state, itkImage})
	    })
	  }

	  render() {

	    const {
	      itkImage
	    } = this.state;

	    var copyImg = {...itkImage, data: []};

	    return (
	      <div className={styles.test}>
	        {JSON.stringify(copyImg)}
	      </div>
	    )
	  }
	}

---

### Display the image using [vtk.js](https://kitware.github.io/vtk-js/index.html)

Example is here [react-med-img-viewer](https://www.npmjs.com/package/react-med-img-viewer)
