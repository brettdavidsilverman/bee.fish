var coordinates = {}

coordinates.Point = function(x, y) {
   this.x = x;
   this.y = y;
}

// Returns cootdinates depending
// on if its a touch or mouse
// event
coordinates.getCoordinates =  function (event) {

   var x, y;
   if ("touches" in event) {
      var touch = event.touches[0];
      x = touch.clientX;
      y = touch.clientY;
   }
   else {
      x = event.clientX;
      y = event.clientY;
   }
   
   x += window.pageXOffset;
   y += window.pageYOffset;
   
   return new coordinates.Point(x, y);
}

