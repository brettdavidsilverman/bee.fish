function JavascriptWriter() {
   
   var indent = 0;
   var writer = this;
   this.javascript = "";
   
   this.openFunction = function(label) {
      this.writeln("function " + this.escapeIdentifier(label) + " (input) {");
      ++this.indent;
   }
   
   this.writeBody = function(body) {
      body.split(";").forEach(
         function(line) {
            writer.writeln(line);
         }
      );
   }
   
   this.closeFunction = function() {
      --this.indent;
      this.writeln("}");
   }
   
   this.writeln = function(line) {
      var indent = "";
      for (var i = 0; i < this.indent; i++)
         indent += "\t";
      this.javascript += indent + line + "\n";
   }
   
   this.escapeIdentifier =function(identifier) {
      return encodeURI(identifier)
         .replace("%", "_");
   }
}