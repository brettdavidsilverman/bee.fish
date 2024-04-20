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
   var startCount = stream.count;
   
   stream.write("1");
   stream.encode(this.valueOf());
   
   var endCount =
      stream.count - startCount;
      
   CHECK(
      "Number.encode end count",
      endCount == 0
   );
   
}

Number.decode = function(stream)
{
   var startCount = stream.count;
   
   CHECK(
      "Number.decode start bit",
      stream.read() == "1"
   );
   
   var number = stream.decode();
   
   var endCount =
      stream.count - startCount;
      
   CHECK(
      "Number.decode end count",
      endCount == 0
   );
   
   return number;
}

String.prototype.toUtf8 = function() {
   const encoder = new TextEncoder("utf8");
   var uint8array = encoder.encode(this);
   return uint8array;
}

String.fromUtf8 = function(uint8array) {
   const decoder = new TextDecoder("utf8");
   var string = decoder.decode(uint8array);
   return string;
}

String.prototype.encode = function(stream)
{

   var utf8 = this.toUtf8();
   
   var startCount = stream.count;
   
   stream.write("1");

   for (var i = 0; i < utf8.length; ++i)
   {
      var charCode = Number(utf8[i]);
      charCode.encode(stream);
   }
   
   stream.write("0");
   
   
   var endCount =
      stream.count - startCount;
      
   CHECK(
      "String.encode end count",
      endCount == 0
   );
   
}

String.decode = function(stream)
{
   var startCount = stream.count;
   
   var utf8 = [];
   
   CHECK(
      "Decode string start bit",
      stream.read() == "1"
   );
   
   var i = 0;
   while (stream.peek() == "1")
   {
      var charCode = Number.decode(stream);
      utf8[i++] = charCode;
   }
   
   CHECK(
      "Decode string end bit",
      stream.read() == "0"
   );
   

   var endCount =
      stream.count - startCount;
      
   CHECK(
      "String.decode end count",
      endCount == 0
   );
   
   var data = Uint8Array.from(utf8);
   
   var string = String.fromUtf8(data);

   return string;
   
}

PowerEncoding.BASE = 2;


