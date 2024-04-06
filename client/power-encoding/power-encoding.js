class PowerEncoding extends Stream {

   
   
   constructor(input) {
      super(input);
   }
   
   //           x
   //          / \
   // x === 5^n + r
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

      // (presuming base 5)...
      //
      // Define 1 as...
      //
      //            1
      //           / \
      // One is 5^0 + 0
  
      
      // So 5 is...
      //
      //     5
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
      this.charCodeAt(i).encode(encoding);
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
      var charCode = Number.decode(encoding);
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
