var movable = {}

// Add the class "movable"
// to elements that you allow to be moved

// The element that is being moved
movable.moving = null;


// set up listeners to document events
// we pair up touch events with mouse events
movable.onload = function(event) {
   
   document.addEventListener("touchstart", movable.onstartmove, true);
   document.addEventListener("mousedown", movable.onstartmove);

   document.addEventListener("touchmove", movable.onmoving,
      {passive: false, capture: true } //set passive to make sure we can cancel the default action of scrolling
   );
   document.addEventListener("mousemove", movable.onmoving);

   document.addEventListener("touchend", movable.onendmove, true);
   document.addEventListener("mouseup", movable.onendmove);
   
   // the click event needs special attention.
   // we want to cancel the event if a move has taken place
   document.addEventListener("click", movable.onclick, true);

}


// Start moving an element
movable.onstartmove = function(event) {
   
   // reset the moving flag
   movable.moving = null;
   movable.moved = false;

   // get the element being moved
   var element = event.target;
      
   // is the element movable?
   if (element.classList.contains("movable")) {
 
      // prevent default from initiating
      // a drag or text select
      event.preventDefault();
      
      movable.moving = element;

      // save the initial position of the target element
      movable.referencePoint = new coordinates.Point(
         element.offsetLeft,
         element.offsetTop
      );
      
      // get the touch/mouse coordinates
      var point = coordinates.getCoordinates(event);
  
      // set the to point of the target element
      movable.startPoint = point;

   }

}

// Move the element
movable.onmoving = function(event) {

   // retrieve the target element from the
   // previously saved moving element
   var element = movable.moving;

   if (!element)
      return;
  
   // Prevent scrolling and refresh
   // on touch screens
   event.preventDefault();

   // get the touch/mouse coordinates
   var currentPoint = coordinates.getCoordinates(event);
   
   // calculate the moving elements new coordinates
   var x = movable.referencePoint.x - movable.startPoint.x + currentPoint.x;
   var y = movable.referencePoint.y - movable.startPoint.y + currentPoint.y;

   // Position the element
   element.style.position = "absolute";
   element.style.left = x + "px";
   element.style.top = y + "px";

   // Flag that this element has been moved
   movable.moved = true;
   
   // create a moving event
   var movingEvent = new Event("moving");
   movingEvent.point = new coordinates.Point(x, y);
   
   // dispatch the event
   movable.moving.dispatchEvent(movingEvent);
}

// Finish moving
movable.onendmove = function(event) {
   
   var element = movable.moving;

   if (!element)
      return;

   // Dont prevent default else
   // the click event wont fire
   
   // clear the moving flag
   movable.moving = null;
}

// Cancels the click event if we detected a move
movable.onclick = function (event) {
   
   if (movable.moved) {
      // since we moved, we need to cancel the click
      event.stopPropagation();
      return;
   }
   
}


// Add events
window.addEventListener("load", movable.onload);