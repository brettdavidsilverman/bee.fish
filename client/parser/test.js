class Match {
   #success = undefined;
   #value = "";
   #inputs;
   
   constructor(...inputs) {
      this.#inputs = inputs;
   }
   
   match(character) {
      this.#value += character;
   }
   
   read(string, end = true) {
     
      var matched;
      var i;
      for (i = 0;
           i < string.length;
           )
      {
         var character = string[i];
         
         var matched =
            this.match(character);
         
         if (matched)
            document.write(
               "{" + 
               escape(character) + 
               "}"
            );
      

            
         if (this.success !=
             undefined)
            return i;
            
         if (matched)
            ++i;
      }
      
      if (end)
         this.readEnd();
         
      return i;
      
   }
   
   readEnd() {
   }
   
   get success() {
      return this.#success;
   }
   
   set success(value) {
      if (value != this.#success)
      {
         this.#success = value;
         if (this.#success)
            this.onsuccess();
      }
   }
   
   onsuccess() {
   }
   
   get value() {
      return this.#value;
   }
   
   get inputs() {
      return this.#inputs;
   }
   
   write(doc) {
      if (!doc)
         doc = document;
         
      if (this.success)
         doc.writeln("Match:" + this.value);
      else
         doc.writeln("No match");
   }
}


            
class Character extends Match {
   #character;
   
   constructor(character) {
      super();
      this.#character = character;
   }
   
   match(character) {
      var matched =
         this.#character === character;
         
      if (matched) {
         super.match(character);
         this.success = true;
      }
      else
         this.success = false;
         
      return matched;
   }
   
}
            
class Range extends Match {
   #minimum;
   #maximum;
   constructor(minimum, maximum) {
      super();
      this.#minimum = minimum;
      this.#maximum = maximum;
   }
   
   match(character) {
   
      var matched =
         (this.#minimum <= character) &&
         (this.#maximum >= character);
         
      if (matched) {
         super.match(character);
         this.success = true;
      }
      else
         this.success = false;
      
      return matched;
   }
   
}
            
class Word extends Match {
   #index = 0;
   #word;
   constructor(word) {
      super();
      this.#word = word;
   }
   
   match(character) {
      var matched =
         this.#word[this.#index] ===
         character;
         
      if (matched)
      {
         super.match(character);
         ++this.#index;
         if (this.#index === this.#word.length) {
            this.success = true;
         }
      }
      else
         this.success = false;
         
      return matched;
   }
   
   
}

            
class And extends Match {
   #index = 0;
   #matches = [];
   constructor(...inputs) {
      super(...inputs);
      if (this.inputs.length === 0) {
         this.success = false;
      }
   }
   
   match(character) {
      var item;
      var matched;
      
      do {
         item =
            this.inputs[this.#index];

         matched =
            item.match(character);
    
         if (item.success) {
         
            this.#matches.push(item);
         
            if (++this.#index  ===
                this.inputs.length)
            {
               this.success = true;
               break;
            }
         }
         else if (item.success == false)
         {
            this.success = false;
         }
         
      } while(item.success && !matched);
         
      return matched;
   }
   
   readEnd(success) {
      while(this.#index <
            this.inputs.length) {
         var item =
            this.inputs[this.#index];

         item.readEnd();
         
         if (item.success) {
            this.#matches.push(item);
            ++this.#index;
         }
         else {
            if (item.success === false)
               this.success = false;
            break;
         }
         
      }
      
      if (this.success === undefined &&
          this.#index ===
          this.inputs.length)
         this.success = true;
         
      super.readEnd();
      
   }
   
   get matches() {
      return this.#matches;
   }
   
   get value() {
      return this.matches.map(
         item => {
            return item.value;
         }
      )
      .join("");
   }
}
            
class Or extends Match {
   #item;
   constructor(...inputs) {
      super(...inputs);
   }
   
   match(character) {
      var matched = false;
 
      for (var i = 0;
           i < this.inputs.length;
           i++)
      {
         var item = this.inputs[i];
         if (item.success === undefined) {
         
            if (item.match(character))
               matched = true;
            
            if (item.success) {
               this.#item = item;
               this.index = i;
               this.success = true;
               return matched;
            }
            
         }
      }
      
      if (!matched)
         this.success = false;
         
      return matched;
   }
   
   readEnd() {
      
      for (var i = 0;
           i < this.inputs.length;
           i++)
      {
         var item = this.inputs[i];
         if (item.success != false) {
            item.readEnd();
            if (item.success) {
               this.#item = item;
               this.index = i;
               this.success = true;
               break;
            }
         }
      }
      
      super.readEnd();

   }
   
   get item() {
      return this.#item;
   }
   
   get value() {
      return this.item.value;
   }
   
}
            
class Not extends Match {
   #match;
   constructor(match) {
      super();
      this.#match = match;
   }
   
   match(character) {
      
      var matched =
         this.#match.match(character);
      
      if (!matched)
         super.match(character);
         
      if (this.#match.success === false) {
         this.success = true;
      }
      else if (this.#match.success)
         this.success = false;

      return !matched;
      
   }
   
   
}
            
class Optional extends Match {
   #match;
   constructor(match) {
      super();
      this.#match = match;
   }
   
   match(character) {
      var matched =
         this.#match.match(character);
      
      if (this.#match.success !=
          undefined) {
         this.success = true;
      }
      
      return matched;
   }
   
   readEnd() {
      this.success = true;
      super.readEnd();
   }
   
   get value() {
      return this.#match.value;
   }
 
}
            
class Repeat extends Match {
   #Match;
   #match;
   #items = [];
  
   constructor(Match) {
      super();
      this.#Match = Match;
      this.#match = new this.#Match();
      
   }
   
   match(character) {
   
      var matched =
         this.#match.match(character);
         
      if (this.#match.success) {
      
         this.#items.push(
            this.#match
         );
            
         this.#match =
            new this.#Match();
           
      }
      else if (this.#match.success ===
               false)
      {
         this.checkSuccess();
         this.#match =
            new this.#Match();
      }
      
      return matched;
      
   }
   
   readEnd() {
      if (this.#match.success ===
          undefined)
      {
         this.#match.readEnd();
         if (this.#match.success) {

            this.#items.push(
               this.#match
            );
         }
      }
      this.checkSuccess();
      super.readEnd();
   }
   
   checkSuccess() {
      
      if (this.#items.length > 0) {
         this.success = true;
      }
      else {
         this.success = false;
      }
   }
   
   get items() {
      return this.#items;
   }
   
   get value() {
      return this.items.map(
         item => {
            return item.value;
         }
      ).join("");
   }
   
}


            
class Capture extends And {

   #object;
   #keys;
   #value;
   constructor(object) {
      super(...Object.values(object));
      this.#object = object;
      this.#keys = Object.keys(object);
   }
   
   match(character) {
      var matched =
         super.match(character);
        
      if (this.success) {
         this.setValues();
      }

      return matched;

   }
   
   readEnd() {
      if (this.success === undefined) {
         super.readEnd();
         if (this.success)
            this.setValues();
      }
   }
   
   setValues() {
      var i = 0;
      var object = this.#object;
      var capture = this;
      super.matches.forEach(
         (item) => {
            var key = capture.#keys[i++];
            object[key] = item;
         }
      );
    
      Object.assign(this, object);
   }
 
   
}
            
class WhitespaceCharacter extends Or {
   constructor() {
      super(
         new Character(" "),
         new Character("\t")
      );
   }
   
   get value() {
      return this.item.value;
   }
}

class Whitespace extends Repeat {

   constructor() {
      super(WhitespaceCharacter);
   }
   
}
            
class Colon extends And {
   constructor() {
       super(
          new Optional(new Whitespace()),
          new Character(":"),
          new Optional(new Whitespace())
       );
   }
   
   get value() {
      return ":";
   }
}
            
class NewLine extends Or {
   static standard = "\r\n";
   constructor() {
      super(
         new And(
            new Character("\r"),
            new Optional(
               new Character("\n")
            )
         ),
         new Character("\n")
      )
   }
   
   get value() {
      if (this.success)
         return NewLine.standard;
      else
         return null;
   }
}
             
class FirstIdentifierCharacter
   extends Or
{
   constructor() {
      super(
         new Range("a", "z"),
         new Range("A", "Z"),
         new Character("_")
      )
   }
}

class SubsequentIdentifierCharacter
   extends Or
{
   constructor() {
      super(
         new Range("0", "9"),
         new Range("a", "z"),
         new Range("A", "Z"),
         new Character("_")
      )
   }
}

class Identifier extends And
{
   #value = "";
   constructor() {
      super(
         new FirstIdentifierCharacter(),
         new Repeat(
            SubsequentIdentifierCharacter
         )
      )
   }
   
   match(character) {
      var matched =
         super.match(character);
      if (matched)
         this.#value += character;
      return matched;
   }
   
   get value() {
      if (this.success)
         return this.#value;
      else
         return null;
   }
   
}
             
class PathCharacter extends Or {
   constructor() {
      super(
         new Character("/"),
         new Character("."),
         new Character("-"),
         new Character("_"),
         new Range("a", "z"),
         new Range("A", "Z")
      )
   }
   
   get value() {
      return this.item.value;
   }
}

class Path extends Repeat {
   constructor() {
      super(
         PathCharacter
      )
   }

}
            
class FirstLine extends Capture {

   constructor() {
      super( {
         verb: new Identifier(),
         whitespace1: new Whitespace(),
         path: new Path(),
         whitespace2: new Whitespace(),
         version:
            new Word("HTTP/1.1"),
         newLine:
            new NewLine()
     } );
   }
}
            
class HeaderPunctuation extends Or {
   constructor() {
      super(
         new Or(
            new NewLine(),
            new Character(":")
         )
      );
   }
}

class HeaderCharacter extends Not {
   constructor() {
      super(
         new HeaderPunctuation()
      );
   }
}

class HeaderName extends Repeat {
   constructor() {
      super(
         HeaderCharacter
      );
   }
}

class HeaderValue extends Repeat {
   constructor() {
      super(
         HeaderCharacter
      );
   }
   
}
            
class HeaderLine extends Capture {
   constructor() {
      super(
         {
            name: new HeaderName(),
            colon: new Colon(),
            _value: new HeaderValue(),
            newLine: new NewLine()
         }
      );
      
   }
   
   write() {
      document.write(
         this.name.value
      );
      document.write(":\t");
      document.writeln(
         this._value.value
      );
   }
   

}

class Headers extends Repeat {
   constructor() {
      super(HeaderLine)
   }
}

            
class Request extends Capture {
   constructor() {
      super(
         {
            firstLine: new FirstLine(),
            headers: new Headers(),
            newLine: new NewLine()
         }
      );
   }
}
            