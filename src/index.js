
const {MedImgReaderBase, FS} = require("./MedImgReader")

const MedImgReader = MedImgReaderBase.extend("MedImgReader", {
    __construct: function() {
        this.__parent.__construct.call(this);
    },
    __destruct: function() {
        this.__parent.__destruct.call(this);
    },
    AddArrayBuffer: function(array_buffer, filename){
    	var stream = FS.open(filename, 'w+');
    	FS.write(stream, array_buffer, 0, $array_buffer.length, 0);
    	FS.close(stream);
    },
    GetStream: function() {
    	var filename = this.GetFilename();
    	var stream = FS.open(filename);
        return Promise.resolve(stream);
    },
    MakeDirectory(directory){
    	try{
    		FS.mkdir(directory);
    	}catch(e){
    		console.error(e);
    	}
    }
});

export default MedImgReader;