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
   

Number.prototype.encode = function(stream)
{
   stream.write("1");
   stream.encode(this.valueOf());
}

Number.decode = function(stream)
{
   CHECK(
      "Number.decode start bit",
      stream.read() == "1"
   );
   
   var number = stream.decode();
   
   return number;
}

String.prototype.encode = function(stream)
{
   stream.write("1");
   
   for (var i = 0; i < this.length; ++i)
   {
      var charCode = this.charCodeAt(i);
      var byte1 = charCode >> 8;
      var byte2 = charCode & 0x00FF;
      byte1.encode(stream);
      byte2.encode(stream);
       //charCode.encode(stream);
   }
   
   stream.write("0");
   
   
   CHECK(
      "String.encode count",
      stream.count == 0
   );
   
}

String.decode = function(stream)
{
   var string = "";
   
   CHECK(
      "Decode string start bit",
      stream.read() == "1"
   );
   
   while (stream.peek() == "1")
   {
      var byte1 = Number.decode(stream);
      var byte2 = Number.decode(stream);
      var charCode = (byte1 << 8) + byte2;
      //var charCode = Number.decode(stream);
      string += String.fromCharCode(charCode);
   }
   
   CHECK(
      "Decode string end bit",
      stream.read() == "0"
   );


   return string;
   
}



PowerEncoding.BASE = 2;
