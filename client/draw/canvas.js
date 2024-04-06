class Canvas extends UserInput {
   _resized = false;
   _context = null;
   _lastDrawTimestamp = null;
   _points = [];
   _thumbnail;
   _inverse;
   _context = null;
   index;
   children;
   matrix;
   toolbox;
   selection;

   static VIBRATE_TIME = 50; // millisecs
   
   constructor(input) {
      super(input ? input.userInput : null);

      var canvas = this;
      
      this.index = 0;

      if (input == undefined)
         input = {}
      
      if (input.matrix == undefined) {
         this.matrix = new Matrix();
      }
      else
         this.matrix =
            Matrix.fromJSON(input.matrix);
         

      var children;
      
      if (input.children == undefined)
         children = {}
      else
         children = input.children;

      children.parent = this;

      this.children =
         new Children(children);

      this._thumbnail = new Image();
      this._thumbnail.onload = function() {
         canvas.draw();
      };
      
      this._thumbnail.src = localStorage.getItem(
         "authentication.thumbnail"
      );
      
      
      this._resized = false;
      
      this._context = null;
   
      window.addEventListener("resize",
         function() {
            if (canvas.element) {
               canvas._resized = false;
               canvas.resize();
               canvas.draw();
            }
         }
      );
      
      this.resize()
   }

   createElement() {
      
      var element =
         document
         .createElement("canvas");
      
      element.style.position =
         "absolute";

      element.style.left = "0px";
      element.style.top = "0px";
      element.style.right = "0px";
      element.style.bottom = "0px";
      element.style.margin = "0";
      element.style.border = "0";
      element.style.zIndex = "1";
      
      document.body.appendChild(
         element
      );
   
      return element;
   }

   get label() {
      return "Canvas";
   }
   
   toJSON() {
      return {
         userInput: super.toJSON(),
         matrix: this.matrix,
         children: this.children.toJSON()
      }
   }

   get context() {

      // Resize if required
      if (!this._resized)
         this.resize();
      
      // Create context if required
      if (!this._context) {

         var context = this.element.getContext("2d");

         context.stack = [];

         context.pushMatrix = function(matrix) {
            this.save();
            this.stack.push(this.matrix);
            this.matrix = matrix;
            this.applyMatrix(matrix);
         }
         
         context.popMatrix = function() {
            this.matrix = this.stack.pop();
            this.restore();
         }

         // Add Push and Pop functions to the 2d context
         context.applyMatrix = function(matrix) {
            this.setTransform(
               matrix.a,
               matrix.b,
               matrix.c,
               matrix.d,
               matrix.e,
               matrix.f
            );
         }

         context.pushMatrix(new Matrix());

         // Cache the context
         this._context = context;
      
      }

      return this._context;
   }
   
   draw(forceDraw = false) {
      
      var element = this.element;
      var canvas = this;
      
      if (forceDraw === true) {
         draw();
         return;
      }
      
      window.requestAnimationFrame(
   
         function(timestamp) {
               
            if ( timestamp <=
                 canvas._lastDrawTimestamp
               )
            return;
         
            canvas._lastDrawTimestamp =
               timestamp;
       
            draw().catch(error => canvas.handleError(error));
         }
      );
      
      async function draw() {

         try {
   
            if (!canvas._resized)
               canvas.resize();

            var context =
               canvas.context;
            
            context.resetTransform();

            context.clearRect(
               0,
               0,
               canvas.width,
               canvas.height
            );

            context.inverse = canvas.inverse;

            context.dimensions = canvas.dimensions.matrixTransform(context.inverse);
            
            // Push the first matrix on the context stack
            context.pushMatrix(canvas.matrix);

            // Draw our children
            await canvas.children.draw(context);

            // Pop the matrix off the stack
            context.popMatrix();

            if (canvas._thumbnail.complete) {
               // Draw the thumbnail
               await drawThumbnail(context);
            }

         }
         catch (error) {
            console.log("Canvas.draw:\n" + error.stack);
         }

      }
      
      async function drawThumbnail(context) {
      
         context.pushMatrix(new Matrix());

         var thumbnail = canvas._thumbnail;
         
         var width = thumbnail.width;
         var height = thumbnail.height;
       
         var destination = new Point(
            {
               x: element.width -
                  width,
               y: 0
            }
         );
         
         context.globalAlpha = 0.5;

         context.drawImage(
            thumbnail,
            0,
            0,
            width,
            height,
            destination.x,
            destination.y,
            width,
            height
         );

         context.popMatrix();
      }

   }
   
   resize() {
        
      var element = this.element;

      // set the canvas elements
      // width in pixels
      element.width =
         window.innerWidth;

      // set width in actual
      // millimeters
      this.width =
         element.width;
    
      // and the height
      // pixels
      element.height = 
         window.innerHeight;

      // millimeters
      this.height =
         element.height;
      
      this.dimensions = new Dimensions(
         {
            min: new Point(
               {
                  x: 0,
                  y: 0
               }
            ),
            max: new Point(
               {
                  x: this.width,
                  y: this.height
               }
            )
         }
      );
      
      // reset the context
      this._context = null;
      
      this._resized = true;
      
      this.scrollToTop();

      //this.children.resize(this);

   }
   
   scrollToTop() {
      document.body.scrollTop = 0; // For Safari
      document.documentElement.scrollTop = 0; // For Chrome, Firefox, IE and Opera
   }
   
   async penDown(point) {
   
      var context = this.context;
      
      context.resetTransform();
      context.lineWidth = 0.3;
      
      context.strokeStyle = "blue";
   
      context.beginPath();
   
      context.moveTo(
         point.x,
         point.y
      );
      
      this._points = [point];

   }
   
   async movePen(point) {

      this._points.push(point);
   
      var context = this.context;
      
      context.lineTo(
         point.x,
         point.y
      );
   
      context.stroke();
   }
   
   async penUp() {

      if (!this._points) {
         return;
      }
      
      var self = this;

      // Convert points to canvas coordinates
      var points = this._points.map(
         point => self.screenToCanvas(point)
      )

      // Get the dimensions
      var dimensions = Dimensions.fromPoints(points);

      // Find its smallest parent
      var parent =
         await this.children.findParent(
            dimensions
         );
      
      if (!parent)
         parent = this;

      // Create the line
      var line = new Line(
         {
            item: {
               parent,
               dimensions
            },
            points
         }
      );


      // Find children inside parent that
      // are contained by the new line
      var childrenMap =
         await parent.children.findChildren(
            line
         );

      
      // Remove children from their parent
      // so we can add it under the new line
      parent.children.forEach(
         (pointer, index, array) => {
            if (pointer && childrenMap.has(pointer.key))
               array[index] = undefined;
         }
      );
      
      // Add the children inside the new line
      childrenMap.forEach(
         child => {
            child.parent = line;
            line.children.push(
               child
            );
         }
      );
      
      // Add the new line inside the parent.
      parent.children.push(line);
            
      // Save and draw.
      line.save();
      parent.save();

      this._points = null;
      
      this.draw();
      
      
   }

   async longPress(point) {

      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit  =
         await this.hitTest(
            point
         );

      if (this.selection && this.selection != hit) {
         this.selection.editing = false;
         this.selection.selected = false;
         this.selection = null;
         console.log("Blur");
      }
        
      if (hit && hit != this.selection) {
         
         this.selection = hit;

         this.selection.selected = true; 
         this.selection.editing = true;
      }
      
      if (this.selection)
         this.toolbox = new Toolbox({parent: this});   

      return this.draw();

      
   }
   
   async click(point) {
   
      window.navigator.vibrate(
         Canvas.VIBRATE_TIME
      );

      var hit  = await this.hitTest(point);
      
      if (this.selection && this.selection != hit) {
         this.selection.editing = false;
         this.selection.selected = false;
         this.selection = null;
      }

      if (hit && hit.click) {
         
         await hit.click(point, this);
         
      }

      this.draw();

      return true;
      
   }

   async hitTest(point) {
      
      var canvasPoint = this.screenToCanvas(point);
      
      var hit = await this.children.hitTest(canvasPoint);
      
      return hit;
   }
   
   remove() {
      document.body.removeChild(
         this.element
      );
      this.element = null;
   }
   
   async transform(matrix) {

      this.matrix.multiplySelf(matrix);

      this._inverse = null;
      
      this.draw();
   }
   
   
   async endTransform() {
      this.save();
   }
   
   get inverse() {
   
      if (this._inverse)
         return this._inverse;
         
      this._inverse =
         this.matrix.inverse();
         
      return this._inverse;
   }

   
   static async load()
   {
      var key = await
         storage.getItem("Canvas");

      var canvas;
      
      if (key)
      {
         console.log("Fetching canvas");
         var id = Id.fromKey(key);
         canvas = await id.load();
      }
      
      if (canvas == undefined)
      {
         console.log("Creating new canvas");
         canvas = new Canvas();
         canvas.save();
         storage.setItem("Canvas", canvas.key);
      }
      
      return canvas;

   }
   
   toString() {
      return this.label;
   }

   screenToCanvas(point) {
      return point.matrixTransform(this.inverse);
   }

   canvasToScreen(point) {
      return point.matrixTransform(this.matrix);
   }

}


