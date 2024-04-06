class Toolbox extends Canvas {

   parent;

   constructor(input) {
      super(input ? input.canvas : null);
      var self = this;

      if (input == undefined)
         input = {}

      this.parent = input.parent;

      input.toolbox = this;

      this.labelTool = new LabelTool(input);
      input.first = this.labelTool;
      input.last = this.labelTool;

      this.valueTool = new ValueTool(input);
      input.last = this.valueTool;

      this.functionTool = new FunctionTool(input);
      input.last = this.functionTool;

      this.connectorTool = new ConnectorTool(input);
      input.last = this.connectorTool;

      this.ifConnectorTool = new IfConnectorTool(input);
      input.last = this.ifConnectorTool;

      this.deleteTool = new DeleteTool(input);
      input.last = this.deleteTool;

      var children = this.children;

      this.tools = [
         this.labelTool,
         this.valueTool,
         this.functionTool,
         this.connectorTool,
         this.ifConnectorTool,
         this.deleteTool
      ];

      this.tools.forEach(
         tool => children.push(tool)
      );

      this.matrix = new Matrix();

   }

   save() {

   }

   
   async click(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit = await this.hitTest(point);

      if (hit && hit.click instanceof Function) {
         hit.click(point).catch(
            error => alert(error.stack)
         );
      }
      else {

         this.remove();

         if (this.parent.selection) {
            this.parent.selection.editing = false;
            this.parent.selection.selected = false;
            this.parent.selection = null;
         }
            
         this.parent.draw();

 

      }

   }
   
   async longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit = await this.hitTest(point);

      if (!hit) {

         this.remove();
         this.parent.selection.selected = false;
         this.parent.selection = null;
         this.parent.draw();
 
      }

   }

   toJSON() {
      return {
         canvas: super.toJSON()
      }
   }

   remove() {
      this.tools.forEach(
         tool => tool.remove()
      )
      this.release();
      super.remove();
   }

   release() {
      super.release();
   }

}