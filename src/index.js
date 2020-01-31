
import MedImgReaderModule from "../build/MedImgReader";
// const MedImgReaderModule = require("../build/MedImgReader");
const {MedImgReaderBase, FS} = MedImgReaderModule();

const MedImgReader = MedImgReaderBase.extend("MedImgReader", {
    __construct: function() {
        this.__parent.__construct.call(this);
    },
    __destruct: function() {
        this.__parent.__destruct.call(this);
    },
    WriteFile: function(filename, array_buffer){
        try{
            FS.writeFile(filename, new Uint8Array(array_buffer), { encoding: 'binary' });
            return Promise.resolve(); 
        }catch(e){
            console.error(e);
        }
    },
    GetStream: function(filename) {
        try{
            var stream = FS.open(filename);
            return Promise.resolve(stream);
        }catch(e){
            console.error(e);
        }
    },
    CloseStream: function(stream){
        try{
            FS.close(stream);
            return Promise.resolve();
        }catch(e){
            console.error();
        }
    },
    MakeDirectory(directory){
    	try{
    		FS.mkdir(directory);
            return Promise.resolve();
    	}catch(e){
    		console.error(e);
    	}
    }
});

export default MedImgReader;