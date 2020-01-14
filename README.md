# med-img-reader

Read a dicom series in the browser using ITK series reader. 
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
	var inputImage = '/path/to/input{.nrrd,.nii.gz,.jpg,.dcm}'
	var outputImage = '/path/to/output{.nrrd,.nii.gz,.jpg,.dcm}'


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
