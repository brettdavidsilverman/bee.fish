class IfConnector extends Connector {
   form;
   width = 100;
   height = 100;
   
   constructor(input) {
      super(input ? input.connector : null);

      var self = this;

      if (input.form){
         this.form = new IfForm(input.form);
      }
      else {
         var center = this.dimensions.center;
         var dimensions = new Dimensions(
            {
               min: {
                  x: center.x - this.width / 2,
                  y: center.y - this.height / 2
               },
               max: {
                  x: center.x + this.width / 2,
                  y: center.y + this.height / 2
               }
            }
         )

         var form = new IfForm(
            {
               functionForm: {
                  form: {
                     html: "true",
                     line: {
                        item: {
                           dimensions
                        }
                     }
                  }
               }
            }
         );
         
         form.points = this.getPoints(form.dimensions);
         form.inputs.push(this.from);
         form.outputs.push(this.to);
         this.dimensions = this.dimensions.include(form.dimensions);
         delete form.label;
         this.form = form;
      }

      this.form.save = async function() {
         return self.save();
      }

      this.click = async function(point, canvas) {
         return self.form.click(point, canvas);
      }

      
   }

   toJSON() {
      return {
         connector: super.toJSON(),
         form: this.form
      }
   }

   async hitTest(point) {
      var hit = await this.form.hitTest(point);
      if (hit)
         return hit;
      return super.hitTest(point);
   }

   async draw(context) {
      var draw = await super.draw(context);
      if (draw) {
         await this.form.draw(context);
      }

      return draw;
   }

   async getSecondColor() {
      var form = this.form;
      var value = form.value;
      if (value == undefined)
         return "orange";
      else if (value == true)
         return "green";
      else
         return "red";
   }

   getPoints(dimensions) {
      var dim = dimensions;
      var points = new Points(
         {
            x: dim.left + dim.width / 2,
            y: dim.top
         },
         {
            x: dim.left + dim.width,
            y: dim.top + dim.height / 2
         },
         {
            x: dim.left + dim.width / 2,
            y: dim.top + dim.height
         },
         {
            x: dim.left,
            y: dim.top + dim.height / 2
         },
         {
            x: dim.left + dim.width / 2,
            y: dim.top
         }
      )
      return points;
   }

   get functionText() {
      var text = "";

      if (this.html != undefined)
         text += "\treturn (" + this.html + ");";
      
      return text;

   }

}