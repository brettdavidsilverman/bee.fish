var Matrix = DOMMatrix;
Matrix.prototype.save = saveObject;
Matrix.fromStorage = matrixFromStorage;
Matrix.prototype.toCSS = Matrix.prototype.toString;
Matrix.prototype.toString = objectToString;
Matrix.prototype.toShorthand =
function(shorthand) {
   var object;
   
   var m = this.toFloat64Array();
      
      
   object = {
      "=": this["="]
              .toShorthand(shorthand),
      "M": m.toShorthand(shorthand)
   }
   
   return object;
}

function matrixFromStorage(object) {
   
   var array = Float64Array.fromStorage(
      object["M"]
   );
   
   var matrix =
      Matrix.fromFloat64Array(array);
   matrix["="] = new Id(object["="]);
   
   return matrix;
}

if (Matrix.prototype.fromMatrix === undefined) {
   Matrix.prototype.fromMatrix =
   function (src) {
      this.m11 = src.m11;
      this.m12 = src.m12;
      this.m13 = src.m13;
      this.m14 = src.m14;
      this.m21 = src.m21;
      this.m22 = src.m22;
      this.m23 = src.m23;
      this.m24 = src.m24;
      this.m31 = src.m31;
      this.m32 = src.m32;
      this.m33 = src.m33;
      this.m34 = src.m34;
      this.m41 = src.m41;
      this.m42 = src.m42;
      this.m43 = src.m43;
      this.m44 = src.m44;
      return this;
   }
}

if (Matrix.fromMatrix === undefined) {
   Matrix.fromMatrix = function(src) {
      var dest = new Matrix();
      dest.fromMatrix(src);
      return dest;
   }
}

if (Matrix.fromFloat64Array === undefined) {
   Matrix.fromFloat64Array = function(src) {
      var dest = new Matrix(src);
      return dest;
   }
}
