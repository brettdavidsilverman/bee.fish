class Editor extends App {

   constructor(input) {
      super(input);
      this.layer = this;
      if (!input.createdButtons)
         this.createButtons();
      
   }
      
   createButtons() {
      var editor = this;

      var buttons = [
         // Set statement
         new Button(
            {
               parent: this,
               label: "f",
               statement: "selection.promptStatement()",
               width: 10,
               height: 10,
               topLeft: this.canvas.topLeft,
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Remove
         new Button(
            {
               parent: this,
               label: "x",
               statement: "selection.confirmRemove()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 20,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black",
            }
         ),
         // Prompt Label
         new Button(
            {
               parent: this,
               label: "a",
               statement: "selection.promptLabel()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 40,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Prompt text
         new Button(
            {
               parent: this,
               label: "T",
               statement: "selection.promptText()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 60,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         ),
         // Prompt console
         new Button(
            {
               parent: this,
               label: "c",
               statement: "selection.promptConsole()",
               width: 10,
               height: 10,
               topLeft: new Point(
                  {
                     x: this.canvas.topLeft.x + 80,
                     y: this.canvas.topLeft.y
                  }
               ),
               lineWidth: 0.5,
               strokeStyle: "black"
            }
         )
      ];
      
      this.children = [];
      buttons.forEach(
         function(button) {
            editor.addChild(button);
         }
      );
    
      this.createdButtons = true;
   }
   
   draw(context) {
      super.draw(context);
   }
   
   
}
   
class Button extends App {

   constructor(input) {
      super(getInput(input));
      
      if (!input.dimensions) {
         this.dimensions = 
            Dimensions.
               fromRectangle(input);
         this.frame.dimensions = this.dimensions;
      }
      
      if (!input.canvas)
         this.canvas = input.parent.canvas;
      
      if (!input.layer)
         this.layer = input.parent.layer;
      
      function getInput(input) {
         input.isTransformed = true;
         return input;
      }
   }
   
   draw(context) {
      
      this.setStyle(this.frame);
      this.frame.draw(context);
      super.draw(context);
 
   }

}