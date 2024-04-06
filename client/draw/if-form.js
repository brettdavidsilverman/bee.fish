class IfForm extends FunctionForm {

   selectedBackgroundColor = "rgba(255, 255, 255, 0)";
   
   constructor(input) {
      super(input ? input.functionForm : null);

   }

   get functionText() {
      var text = "";

      if (this.html != undefined)
         text += "\treturn (" + this.html + ");";

      return text;

   }

   toJSON() {
      return {
         functionForm: super.toJSON()
      }
   }

   async draw(context) {
      
      var color = "black";

      if (this.f) {
         if (this.value == undefined)
            color = "orange";
         else if (this.value == true)
            color = "green";
         else if (this.value == false)
            color = "red";            
      }
      else
         color = "red";

      this.strokeStyle = 
         this.borderColor = color;

      return super.draw(context);
   }

}