class FunctionTool extends ToolboxItem {

   form;
   editing = false;

   constructor(input) {
      super(input);

      if (this.selection instanceof Form) {
         this.form = this.selection;
      }
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "yellow";
      context.strokeStyle = "yellow";

      this.drawText(context, "Italic", 40, "Times New Roman", "f(x)", true, true);

   }

   async click(point) {

      if (!(this.selection instanceof Line)) {
         alert("Can only convert lines to forms");
         return;
      }

      if (this.form == undefined) {
   
         if (!confirm("Convert line to form?"))
            return;

         // Create the form
         var line = this.selection;
         var parent = line.parent;
         var form = new FunctionForm(
            {
               form: {
                  line
               }
            }
         );
         
         form.dimensions = line.dimensions;
         form.index = line.index;
         form.label = line.label;
         form.parent = parent;
         parent.children.remove(line);
         parent.children.push(form);
         form.save();
         parent.save();
         form.show();
         this.selection = form;
         this.form = form;
         this.form.editing = true;
      }
      else {
         this.form.editing = !this.form.editing;
      }

      this.toolbox.draw();
      this.toolbox.parent.draw();
   }

   remove() {
      if (this.form) {
         this.form.editing = false;
      }
      super.remove();
   }


}
