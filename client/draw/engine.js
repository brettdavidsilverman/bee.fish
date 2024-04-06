function Engine(input) {
   Object.assign(this, input);
      
   if (!this.start)
      this.start = this.canvas.selection;
      
   if (!this.stack) {
      this.stack = [];
      if (this.start.f)
         this.stack.push(this.start.f);
   }
   
   if (!this.map)
      this.map = new Map();
   var input = null;
   
   this.run = function() {
      while(this.stack.length) {
         var object = stack.pop();
         if (object.f) {
            object.f();
            var next = object.to.map(
               function(next) {
                  if (next.f && next.f())
                     return true;
                  else
                     return false;
               }
            );
            next.forEach(
               function(nextConnector) {
                  this.stack.push(
                     nextConnector.to
                  );
               }
            );
         }
      }
   }
}