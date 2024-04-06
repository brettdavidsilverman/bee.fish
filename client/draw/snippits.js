// Load image

var app = this;
var image = new Image();
image.onload = function() {
   app.connectOutputs(app, image);
}
image.src = "/feebee.jpg";


// draw image
var image = input.image;
this.ondraw = function(context) {
   var dim = this.dimensions;
   context.drawImage(
      image,
      0,
      0,
      image.width,
      image.height,
      dim.left,
      dim.top,
      dim.width,
      dim.height 
   );
   return true;
}

image = new Image();
image.src = "/feebee.jpg";
image.onload = function()
{
   canvas.draw();
}



this.ondraw = function(context)
{
   var width = 
      this.dimensions.width;



   var height = 
      this.dimensions.height;


   context.drawImage(
      image,
      0, 0,
      width,
      height
   );
}