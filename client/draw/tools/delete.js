class DeleteTool extends ToolboxItem {

   constructor(input) {
      super(input);

   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "red";
      context.strokeStyle = "red";

      this.drawText(context, "", 40, "Arial", "X", true, true);

   }

   async click(point) {
      if (confirm("Delete " + this.selection + "?")) {
         var parent = this.selection.parent;
         this.selection.remove();
         parent.save();
         this.selection = null;
         this.toolbox.parent.draw();
         this.toolbox.remove();
      }
   }
}
