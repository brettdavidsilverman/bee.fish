class App extends Drawing {
   _label;
   constructor(input) {
      super(input);

      Object.assign(this, input);
      
      if (!input.inConnectors)
         this.inConnectors = [];
   
      if (!input.outConnectors)
         this.outConnectors = [];
      
      if (!input.input)
         this.input = {}
      
      if (!("label" in input))
         this.label = this["="].name;
      
      if (!this.statement)
         this.statement = "this.promptStatement()";
   }
   
   click(point) {

      if (this.f) {
         this.runFunction();
      }

      return true;
   }
   
   longPress(point) {
     
      // Editor
      if (!this.editor) {
          
         this.editor = new Editor(
            {
               canvas
            }
         );

      }
      
      this.canvas.layers.push(
         this.editor
      );
     
      selection = this;
      
      //this.editor.point = point;

      this.canvas.draw();
      
   }

   ondraw(context) {
      return true;
   }
   
   async draw(context) {
      
      var _super = await super.draw(context);
      if (!_super)
         return false;
       
      context.save();

      var _ondraw = await this.ondraw(context);
      if (!_ondraw)
      {
         context.restore();
         return false;
      }
      
      context.restore();
      
      var parent = await this.parent;
      if (parent != null)
         await this.drawLabel(context);
      
      
      return true;
   }
   
   async drawLabel(context) {
      var label = this.label;
      if (label == null)
         return;
      
      context.save();
      
      context.fillStyle = "black";
      context.strokeStyle = "black";

      var dimensions = await
         this.dimensions;
            
      var topLeft = await
         dimensions.topLeft;
         
      var scale = context._scale;
      var fontSize = 3 / scale; //mm
      
      context.font =
         String(fontSize) + "mm" +
         " Courier new";
      
      context.scale(1, -1);

      var width = context.measureText(
         this.label
      ).width;
      
      var x = topLeft.x        +
        dimensions.width   / 2 -
        width              / 2;
        
      var y = topLeft.y        -
         dimensions.height / 2 -
         fontSize;
         
      context.beginPath();
         
      context.fillText(
         label,
         x,
         -y
      );
      context.lineWidth = 0.1 / context._scale;
      
      context.stroke();
         
      context.restore();
   }
   
   
   // set the fill style 
   // and stroke style for this 
   // context
   setStyle(app) {
   
      const alpha = 1;
      app.strokeStyle = "black";
      if ( this.selected ) {
         // yellow
         app.fillStyle = rgba(255, 255, 127, alpha);
         
      }
      else if (this.error != null) {
         // red
         app.fillStyle = rgba(255, 0, 0, alpha);
      }
      else if (this.f  &&
               this.output == null) {
         // orange
         app.fillStyle = rgba(255, 127, 0, alpha);
      }
      else if (this.f != null &&
                  this.output != null) {
         // green
         app.fillStyle = rgba(0, 255, 0, alpha);
      }
      else if (this.f == null) {
         // black
         app.fillStyle = rgba(127, 127, 127, alpha);
      }
      else
         throw new Error("Invalid state");
      
      app.lineWidth = 0.5;
      
      function rgba(red, green, blue, alpha) {
         var string =
            "rgba(" +
            red.toString() + "," +
            green.toString() + "," +
            blue.toString() + "," +
            alpha.toString() + ")";
         return string;
      }
   }
   
   promptStatement() {
      
      var app = this;
      var code = new Code(
         {
            statement: app.statement,
            save: function(statement) {
               app.statement = statement;
            }
         }
      );
      
   }
   
   promptLabel() {
   
      var label = this.label;
      
      if (label === null)
         label = "";
         
      label = prompt("Label?", label);
 
      if (label === null)
         return false;
         
      if (label == "")
         this.label = null;
      else
         this.label = label;
         
      this.canvas.draw();
      
      return true;
   }
   
   promptText() {
      var text = prompt("Text?");
      if (text == null)
         return false;
      var object = new Text(
         {
            text: text,
            point: this.editor.point,
            parent: this
         }
      );
      this.addChild(object);
      return true;
   }

   promptConsole() {
      if (this.lastCommand == null)
         this.lastCommand = "console.log(this)";
         
      var command = prompt("Command?", this.lastCommand);
      if (command == null)
         return false;
      
      this.lastCommand = command;
      
      eval(command);
      
      return true;
   }
   
   createFunction(keepOutput) {
   
      delete this.f;
      delete this.error;
      
      if (!keepOutput)
         delete this.output;

      if (this.statement === undefined)
         return false;
    
      try {
         
         this.f = new Function(
            "input",
            this.statement
         );
      }
      catch (error) {
         this.error = error;
         alert(error);
      }
      
      return true;
   }
   
   runFunction() {
   
      lastApp = this;
      
      delete this.output;
      delete this.error;

      var input = this.input;
      
      try {
         var output = this.f(input);
      }
      catch (error) {
         this.error = error;
         alert(error);
      }
      
      this.connectOutputs(output);
      
      this.canvas.draw();
      
      return true;
   }
   
   addOutConnector(connector) {
      this.outConnectors.push(connector);
   }
   
   addInConnector(connector) {
      this.inConnectors.push(connector);
   }
   
   removeOutConnector(connector) {
      this.removeItem(this.outConnectors, connector);
   }
   
   removeInConnector(connector) {
      this.removeItem(this.inConnectors, connector);
   }
   
   removeItem(array, item) {
      var index = array.indexOf(item);
      array.splice(index, 1);
   }
   
   connectOutputs(output) {
      this.outConnectors.forEach(
         function(connector) {
            connector.connectOutput(output);
         }
      );
      this.output = output;
      this.canvas.draw();
   }
   
   get statement() {
      return this._statement;
   }
   
   set statement(value) {
      this._statement = value;
      this.createFunction();
   }

}