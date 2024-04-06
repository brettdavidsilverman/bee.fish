class Stream
{
   constructor(input)
   {
      this.index = 0;
      this.count = 0;
      this.bits = "";
      
      if (input instanceof Stream)
         this.bits = input.bits;
      else if (typeof input == "string")
      {
         this.bits = this._getBitsFromString(
            input
         );
      }
      
      if (this.bits === undefined)
         this.bits = "";
   }
   
   copy()
   {
      var stream = new Stream(this);

      return stream;
   }
   
   peek()
   {
      var bit = this.bits[this.index];
      return bit;
   }
   
   read()
   {
      
      var bit = this.peek();
      
      ++this.index;
      
      if (bit === "1")
         this.count++;
      else
         this.count--;
      
      return bit;
   }

   write(bits)
   {
      this.bits += bits;
   }
   
   chain(downStream)
   {
      var ourRead = this.read
      this.read = function() {
         var bit = ourRead.call(this);
         downStream.write(bit);
      }
   }
   
   toString()
   {

      var bits = this.bits;
      
      var string = "";
      
      for ( var i = 0;
            i < bits.length;
            i += Stream.BITS_PER_CHARACTER )
      {
         var chunk = bits.substr(
            i, Stream.BITS_PER_CHARACTER
         );

         /*
         chunk = chunk.padEnd(
            Stream.BITS_PER_CHARACTER,
            "0"
         );
         */
         for (var x = chunk.length; x < Stream.BITS_PER_CHARACTER; x++) {
            chunk += "0";
         }

         var number = parseInt(chunk, 2);
         var c = String.fromCharCode(number);
         string += c;
      }
      
      return string;
   }
   
   _getBitsFromString(string)
   {
   
      var bits = "";
      for (var i = 0; i < string.length; ++i)
      {
         var number = string.charCodeAt(i);
         var chunk = number.toString(2);

         /*
         chunk = chunk.padStart(
            Stream.BITS_PER_CHARACTER,
            "0"
         );
         */
         for (var x = chunk.length; x < Stream.BITS_PER_CHARACTER; x++) {
            chunk = "0" + chunk;
         };

         bits += chunk;
      }
     
      return bits;
   }
   
   
   
}

Stream.BITS_PER_CHARACTER = 8;
