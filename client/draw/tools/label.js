class LabelTool extends ToolboxItem {

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);

      context.lineWidth = 1;
      context.fillStyle = "green";
      context.strokeStyle = "green";

      this.drawText(context, "", 40, "Arial", "abc", true, true);

   }

   async click(point) {
      var selection = this.selection;

      var label = selection.label;

      if (label == null)
         label = "label";
      
      label = prompt("Label", label);
      
      if (label == null)
         return;

      selection.label = label;
      selection.save();
      this.toolbox.parent.draw();
      this.toolbox.remove();
   }

}
