class FunctionForm extends Form {

   constructor(input) {
      super(input ? input.form : null);
   }

   toJSON() {
      return {
         form: super.toJSON()
      }
   }


   get functionText() {
      
      var text = "";

      if (this.html != undefined)
         text += this.html + ";";

      return text;

   }

   async click(point, canvas) {
      var output = undefined;
      if (this.f == undefined) {
         await this.compile();
      }

      if (this.f) {
         var inputs = await this.inputs.all();
         try {
            output = this.f(...inputs.map(input => input.value));
            if (confirm(output)) {
               this.value = output;
            }
            this.save();
         }
         catch (error) {
            alert("Error running f:\n" + error);
         }
         canvas.draw();
      }


      return output;

   }

   async draw(context) {
      if (this.f) {
         if (this.value != undefined)
            this.strokeStyle = "green";
         else
            this.strokeStyle = "orange";
      }
      else
         this.strokeStyle = "red";
      return super.draw(context);
   }
   
}