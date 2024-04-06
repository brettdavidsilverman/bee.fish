class PowerEncoding extends Stream {

   
   
   constructor(input) {
      super(input);
   }
   
   //           x
   //          / \
   // x === 2^n + r
   //
   // repeat for n (exponent)
   // and r (remainder)
   encode(x) {
      if (isNaN(x)) {
         var error =
            "PowerEncoding.encode: Not a number: " +
            String(x);
         throw new Error(error);
      }
         
      // Define zero as "0"
      if (x === 0) {
         this.write("0");
         return this;
      }

      // (presuming base 2)...
      //
      // Define 1 as...
      //
      //            1
      //           / \
      // One is 2^0 + 0
  
      
      // So 2 is...
      //
      //     1
      //    / \
      //   1   0
      //  /\  
      // 0  0   

      // Open a new branch
      this.write("1");
      
      // Encode the exponent
      var _exponent = exponent(x);
      this.encode(_exponent);
 
      // Encode the remainder
      var _remainder = remainder(
         x,
         _exponent
      );
      this.encode(_remainder);
      
      return this;
      
      function exponent(x)
      {
         var result =
            Math.floor(
               Math.log(x) /
               Math.log(PowerEncoding.BASE)
            );
         return result;
         
      }
   
      function remainder(x, exponent) {
         var result =
            x -
            Math.pow(
               PowerEncoding.BASE, exponent
            );
         return result;
      }
   
      
   }
   
   decode()
   {
   
      var bit = this.read();

      if ( bit === "0" )
      {
         return 0;
      }
      
      var exponent =
         this.decode();
         
      var remainder =
         this.decode();
         
      var value =
         Math.pow(
            PowerEncoding.BASE, exponent
         ) +
         remainder;
      
      return value;
   }
   
   
}
   

Number.prototype.encode = function(encoding)
{
   encoding.write("1");
   encoding.encode(this.valueOf());
}

Number.decode = function(encoding)
{
   CHECK(
      "Number.decode start bit",
      encoding.read() == "1"
   );
   
   var number = encoding.decode();
   
   return number;
}

String.prototype.encode = function(encoding)
{
   encoding.write("1");
   
   for (var i = 0; i < this.length; ++i)
   {
      var charCode = this.charCodeAt(i);
      var byte1 = charCode >> 8;
      var byte2 = charCode & 0x00FF;
      alert([byte1, byte2]);
      byte1.encode(encoding);
      byte2.encode(encoding);
   }
   
   encoding.write("0");
}

String.decode = function(encoding)
{
   var string = "";
   
   CHECK(
      "Decode string start bit",
      encoding.read() == "1"
   );
   
   while (encoding.peek() == "1")
   {
      var byte1 = Number.decode(encoding);
      var byte2 = Number.decode(encoding);
      var charCode = (byte1 << 8) + byte2;
      string += String.fromCharCode(charCode);
   }
   
   CHECK(
      "Decode string end bit",
      encoding.read() == "0"
   );
   
   return string;
   
}

class CheckError extends Error
{   
   constructor(label)
   {
      super("Check failed for " + label);
      this.label = label;
   }
   
}

function CHECK(label, bool)
{
   if (bool == false)
   {
      throw new CheckError(label);
   }
}

PowerEncoding.BASE = 2;
