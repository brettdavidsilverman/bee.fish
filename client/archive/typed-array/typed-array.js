definetypedArray(Int8Array);
definetypedArray(Uint8Array);
definetypedArray(Uint8ClampedArray);
definetypedArray(Int16Array);
definetypedArray(Uint16Array);
definetypedArray(Int32Array);
definetypedArray(Uint32Array);
definetypedArray(Float32Array);
definetypedArray(Float64Array);
definetypedArray(BigInt64Array);
definetypedArray(BigUint64Array);

function definetypedArray(type) {
   
   type.prototype.toString = objectToString;
   type.prototype.toShorthand = arrayToShorthand;
   type.fromStorage = arrayFromStorage;
   type.prototype.save = saveObject;
   type.prototype.saveChildren = arraySaveChildren;
   type.prototype.encode = encodetypedArray;
   type.decode = decodetypedArray;
   defineId(type);
}

var getEndianIndex;

function encodetypedArray(shorthand) {

   if (shorthand & Shorthand.HUMAN)
      return Array.from(this);
      
   var dataView = new DataView(this.buffer);
   var chars = "";
   
   for ( var i = 0;
         i < this.length *
             this.BYTES_PER_ELEMENT;
         ++i )
   {
      var x = getEndianIndex(
         i,
         this.BYTES_PER_ELEMENT
      );
      var c = dataView.getUint8(x);
      chars += String.fromCharCode(c);
   }
   
   return btoa(chars);
}

function decodetypedArray(data, id) {
   var chars = atob(data);
   var buffer = new ArrayBuffer(chars.length);
   var dataView = new DataView(buffer);
   for ( var i = 0;
         i < chars.length;
         ++i )
   {
      var c = chars.charCodeAt(i);
      var x = getEndianIndex(i, id.type.BYTES_PER_ELEMENT);
      dataView.setUint8(x, c);
   }
   
   return new id.type(buffer);
}

var littleEndian =
   checkEndian() === "little endian";

function checkEndian() {
    var arrayBuffer = new ArrayBuffer(2);
    var uint8Array = new Uint8Array(arrayBuffer);
    var uint16Array = new Uint16Array(arrayBuffer);
    uint8Array[0] = 0xAA; // set first byte
    uint8Array[1] = 0xBB; // set second byte
    if(uint16Array[0] === 0xBBAA)
       return "little endian";
    if(uint16Array[0] === 0xAABB)
       return "big endian";
    else throw new Error("Something crazy just happened");
}

if (littleEndian) {
   getEndianIndex = function(i, bytesPerElement) {
         var count =
            Math.floor(i / bytesPerElement);
         var bytes = i % bytesPerElement;
         return count * bytesPerElement +
            bytesPerElement - bytes - 1;
      }
   }
else
   getEndianIndex = function(i) {
      return i;
   }



