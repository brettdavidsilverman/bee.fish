class Item extends Id {
   dimensions;
   children;
   label;
   labelColor = "black";
   value;
   valueColor = "black";
   backgroundColor;
   selectedBackgroundColor = "rgba(256, 256, 0, 0.5)";
   borderColor = null;
   selectedBorderColor = "yellow";
   index;
   parent;

   inputs;
   outputs;

   visible = false;
   selected = false;
   matrix = new Matrix();

   static _index = 0;
      
   constructor(input) {
      super(input ? input.id : null);
            
      if (input == undefined)
         input = {};

      this.parent = input.parent;

      this.children =
         createChildren(this, input.children);

      if (input.index == undefined)
         this.index = this.getNextIndex();
      else {
         this.index = Number(input.index);
         if (this.index > Item._index)
            Item._index = this.index;
      }

      if (input.label != undefined)
         this.label = input.label;

      if (this.label == undefined && this.index != undefined)
         this.label = String(this.index);

      
      this.value = input.value;

      if (input.dimensions)
         this.dimensions =
            new Dimensions(input.dimensions);
      
      this.inputs  = new Collection(input.inputs);
      this.outputs = new Collection(input.outputs);

      this.inputConnectors = new Collection(input.inputConnectors);
      this.outputConnectors = new Collection(input.outputConnectors);
      
      if (input.labelColor != undefined)
         this.labelColor = input.labelColor;

      if (input.valueColor != undefined)
         this.valueColor = input.valueColor;

      if (input.backgroundColor != undefined)
         this.backgroundColor = input.backgroundColor;

      if (input.selectedBackgroundColor != undefined)
         this.selectedBackgroundColor = input.selectedBackgroundColor;

      if (input.borderColor != undefined)
         this.borderColor = input.borderColor;

      if (input.selectedBorderColor != undefined)
         this.selectedBorderColor = input.selectedBorderColor;
      if (this.visible)
         this.show();

      this.compile(false);

      function createChildren(parent, items) {
         if (items == undefined)
            items = {}

         items.parent = parent;

         return new Children(items);
      }

   }
   
   getNextIndex() {
      return ++Item._index;
   }

   static createIdentifier(label) {
      
      var identifier = "";

      if (isNumeric(label[0])) {
         identifier = "_";
      }

      for (var i = 0; i < label.length; ++i) {
         if (isWhitespace(label[i]))
            identifier += "_";
         else
            identifier += label[i];
      }

      return identifier;

      function isWhitespace(character) {
         return ( character == " " ||
                  character == "\r" ||
                  character == "\n" ||
                  character == "\t" )
      }

      function isNumeric(character) {
         return character >= "0" && character <= "9";
      }

   }

   blur() {

   }
   
   async hitTest(point) {

      var hit =
         this.dimensions.isPointInside(point);
           
      if (hit)
      {
         var child =
            await this.children.hitTest(
               point
            );
        
         if (child)
            return child;
      
         return this;
      }
      /*
      var inputConnectors = await this.inputConnectors.all();
      
      for (var i in inputConnectors) {
         var inputConnector = inputConnectors[i];
         hit = await inputConnector.hitTest(point);
         if (hit) 
            return hit;
      }
      */
      return null;
   }
   
   async findParent(dimensions) {

      var contains =
         this.dimensions.contains(dimensions);
        
      if (contains) {
      
         var parent =
            await this.children.findParent(
               dimensions
            );
         
         if (parent)
            return parent;
            
         return this;
      }
      
      return null;
   }
   
   
   
   isChild(parent) {
      return parent.dimensions.contains(this.dimensions);
   }
   
   async draw(context) {
      
      if ( this.dimensions.intersects(context.dimensions) ) {
         context.save();
         
         var item = this;

         if (!this.visible)
            this.show();
         
         var rectangle = new Rectangle(this);
         if (this.selected) {
            rectangle.fillStyle = this.selectedBackgroundColor;
            rectangle.strokeStyle = this.selectedBorderColor;
         }
         else {
            rectangle.fillStyle = this.backgroundColor;
            rectangle.strokeStyle = this.borderColor;
         }

         await rectangle.draw(context);

         await this.children.draw(context);
   
         //await this.inputConnectors.draw(context);

         this.drawLabel(context);
         this.drawValue(context);         

         context.restore();

         return true;

      }
      else if (this.visible)
         this.hide();

      return false;
   }
   
   drawLabel(context) {
      if (this.label != undefined) {
         context.fillStyle = 
            context.strokeStyle =
               this.labelColor;
         this.drawText(context, "", 20, "Courier New", this.label, false);
      }
   }

   drawValue(context) {
      if (this.value != undefined) {
         if (this.value instanceof Item)
            this.value.draw(context);
         else {
            context.fillStyle = 
               context.strokeStyle =
                  this.valueColor;
            this.drawText(context, "", 20, "Courier New", String(this.value));
         }
      }
   }

   drawText(context, style, size, font, text, center = true, scale = false) {
      context.lineWidth = 1 / context.matrix.scale();
      var fontSize = size;
      
      if (scale == false)
         fontSize /= context.matrix.scale();

      var fullFont =
          style + " " + 
          fontSize + "px " +
          font;

      context.font = fullFont;
      var start;
      
      if (center) {
         context.textAlign    = "center";
         context.textBaseline = "middle";
         start = this.dimensions.center;
      }
      else {
         start = this.dimensions.min;
      }

      context.fillText(text, start.x, start.y);
   }

   show() {
      Pointer.map.set(this.key, this);
      console.log("Show:" + Pointer.map.size);
      this.visible = true;
   }

   async hide() {
      this.visible = false;
      this.release();
      console.log("Hide:" + Pointer.map.size);
   }

   async remove() {
         
      var self = this;

      // Recursively remove our children
      await this.children.removeAll();
      
      this.parent.children.remove(this);

      this.release();

      // Remove ourself
      return await super.remove();
      
   }

   release() {
      this.inputs.release();
      this.outputs.release();
      this.inputConnectors.release();
      this.outputConnectors.release();
      this.children.release();
      Pointer.map.delete(this.key);
      console.log("Release:" + Pointer.map.size);
      super.release();

   }

   async click(point, canvas) {
      alert("Click..." + this.name + ":" + this.label + "." + this.parent.name + ":" + this.parent.label);

      var value = this.value;

      if (value == null)
         value = "";
      
      value = prompt("Value", value);
      
      if (value == null)
         return;

      this.value = value;
      this.save();

   }

   
   toJSON() {
      return {
         id : super.toJSON(),
         index: this.index,
         label: this.label,
         value: this.value,
         dimensions: this.dimensions,
         matrix: this.matrix,
         children: this.children,
         inputs: this.inputs,
         outputs: this.outputs,
         inputConnectors: this.inputConnectors,
         outputConnectors: this.outputConnectors
      }
   }

   async compileForEngine() {

      var f = await this.compile();

      if (confirm(String(f)))
         this.f = f;
   }

   get functionText() {
      return "\treturn (" + this.value + ");\n";
   }

   async compile(alertOnError = true) {

      var text = this.functionText;
      var inputs = await this.inputs.all();

      try {
         var f = new Function(
            inputs.map(input => Item.createIdentifier(input.label)),
            text
         );
         this.f = f;
      }
      catch (error) {
         if (alertOnError)
            alert("Error compiling:\n" + text + "\n" + error);
      }
   }



   toString()  {
      return this.label;
   }


}