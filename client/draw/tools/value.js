class ValueTool extends ToolboxItem {

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);
      
      context.lineWidth = 1;
      context.fillStyle = "blue";
      context.strokeStyle = "blue";

      this.drawText(context, "", 40, "Arial", "𝓧", true, true);
   }

   async click(point) {


      var selection = this.selection;

      var value = selection.value;
      
      if (value == null)
         value = "𝓧";
      
      value = prompt("Value", value);
      
      if (value == null)
         return;

      if (value === "")
         value = undefined;
         
      selection.value = value;
      selection.save();
      this.toolbox.parent.draw();
      this.toolbox.remove();
   }

}
