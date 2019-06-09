# med-img-reader

Read a dicom series in the browser using ITK series reader. 
[Viewer example](https://hpc.medimg-ai.com)

## Installation

----
	npm install med-img-reader
----

## Usage example

### First things first

---
	const {FS, MedImgReader} = require('med-img-reader');
---

### Example how to get some dicom files and write them in the browser

---
	getDicomFiles(){
	    const self = this;

	    var series_dir = '/some_dir';

	    try{
	    //This package uses FS from emscripten, for more information on FS read the documentation at https://emscripten.org/docs/api_reference/Filesystem-API.html
	      FS.stat(series_dir);
	    }catch(e){
	      FS.mkdir(series_dir);
	    }
		
		//Use your own mechanism to request the dicom file from your server. You can use 'axios' for example or http from angular etc.
	    return self.getDicomBufferSomeHow()
		.then(function(res){
		  if(res.data){
		    var img_filepath = series_dir + '/mydcmfile';
		    try{
		      //This will write the dicom file in the FS file system in the browser
		      FS.writeFile(img_filepath, new Uint8Array(res.data), { encoding: 'binary' });  
		    }catch(e){
		      console.error(e);
		    }
		  }else{
		    console.error(res);
		  }
		});
	}
---

### Example how to read the dicom series

---
	readSeries(series_dir){
	    try{
	      const medImgReader = new MedImgReader();
	      medImgReader.SetDirectory(series_dir);
	      medImgReader.ReadDICOMDirectory();    
	      return Promise.resolve(medImgReader);
	    }catch(e){
	      return Promise.reject(e);
	    }
	}
---

### Example to manipulate the image data

---
	getTheActualImageData(medImgReader) {
	    medImgReader.GetOrigin();
      	medImgReader.GetSpacing();
	    medImgReader.GetImageBuffer();//This is the array with the actual values of the dicom series
	    medImgReader.GetNumberOfComponentsPerPixel();
	    medImgReader.GetDirection();
	    medImgReader.GetDimensions();
	}
---