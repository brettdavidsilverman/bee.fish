class UserInput extends Id {

   _touchPoints = null;

   _startPoint = null;
   _penMoved = false;
   _drawing = false;
   _transforming = false;
   _touchCount = 0;
   element;
   
   static MAX_MOVE = 18; // Pixels
   static LONG_PRESS_TIME = 300; // millis
   static ZOOM_INTENSITY = 0.3;
   static TRANSFORM_TIMEOUT = 500; // seconds

   constructor(input) {
      super(input && input.id ? input.id : null);  
      this.element = this.createElement();
      var element = this.element;
      
      var longPressTimer = null;
      var longPressPoint = null;
      var endTransformTimer = null;
      var userInput = this;
      
      setEvents(element);
      
      function setEvents(element) {
         element.ontouchstart = onstart;
         element.onmousedown  = onstart;

         element.ontouchmove = onmove;
         element.onmousemove = onmove;
         
         element.ontouchend    = onend;
         element.ontouchcancel = onend;
         element.onmouseup     = onend;
         element.onmouseleave  = onend;
      
         element.onwheel =
            onmousezoom;
      }
  
   
      function onstart(event) {

         event.preventDefault();
         
         userInput._touchCount = 1;
         
         if (event.touches)
            userInput._touchCount =
               event.touches.length;

         if (userInput._touchCount == 2) {
            // two or more fingers,
            // start scale/translate
            // mode
            userInput._touchPoints =
               [
                  getPoint(event, 0),
                  getPoint(event, 1)
               ];
            
            // cancel any selected 
            // drawings
            userInput._drawing = false;
            userInput._transforming = true;
            
            // cancel long press timer
            clearLongPressTimer(
               "multiple"
            );
            
            return;
         }
         
         if (userInput._touchCount == 1) {
            // cancel selected transform
            userInput._touchPoints = null;
            userInput._transforming = false;
         
            // get the point and start
            // drawing
            var point = getPoint(
               event, 0
            );
   
            userInput._startPoint =
               point.copy();
            userInput._drawing = true;
            userInput._penMoved = false;
         
            userInput.penDown(point);
         
            // Start the timer for long
            // touch
            longPressTimer =
               window.setTimeout(
                  onlongpress,
                  UserInput.LONG_PRESS_TIME
               );
               
            longPressPoint =
               userInput._startPoint.copy();
               
         }

      }
      
      function onmove(event) {

         event.preventDefault();
         
         if (userInput._transforming) {

             // get the next two points
             // for translate/scale
             userInput._touchPoints.push(
                getPoint(event, 0)
             );
         
             userInput._touchPoints.push(
                getPoint(event, 1)
             );

             // perform the transform
             touchTransform();
      
             // shift the two points
             // to replace the first ones
             userInput._touchPoints
                 .shift();
             
             userInput._touchPoints
                 .shift();
      
         }
         else if (userInput._drawing) {
      
            // get the next point
            // and move the pen to
            // there
            var point = getPoint(event, 0);
               
            userInput
               .movePen(point)
               .catch(
                  (error) => userInput.handleError(error)
               );
            
            // If we have moved greater
            // then 5 pix then
            // cancel the long press
            // timer
            if ( !userInput._penMoved &&
                 Point.distance(
                    point,
                    userInput._startPoint
                 ) > UserInput.MAX_MOVE )
            {
            
               clearLongPressTimer(
                  "move"
               );
               
               userInput._penMoved = true;
            }
            
   
         }
      }

      function onend(event) {
         event.preventDefault();
      
         clearLongPressTimer("end");

         if (event.touches)
            userInput._touchCount =
               event.touches.length;
         else
            userInput._touchCount = 0;
            
         if (userInput._transforming) {
            // stop transforming
            userInput._touchPoints = null;
            userInput._transforming = false;
            userInput.endTransform()
            .catch(
               (error) => userInput.handleError(error)
            );
         }
         else if (userInput._drawing) {
            
            var clicked = false;
            if (!userInput._penMoved) {
               clicked =
                  userInput.click(
                     userInput._startPoint
                  ).catch(error => userInput.handleError(error));
            }
            
            if (!clicked) {
               userInput.penUp()
               .catch(
                  (error) =>userInput.handleError(error)
               );
            }
         
            // stop drawing
            userInput._drawing = false;
         
         }
      
      }
      
      // mouse wheel used for zooming
      function onmousezoom(event) {
      
         event.preventDefault();
         
         clearTransformTimer();

         var point = getPoint(event);
      
      
         // transform and copy
         // point to canvas
         // coordinates
         var inverse = userInput.inverse;

         point = point.matrixTransform(
            inverse
         );

    
         // Normalize wheel to +1 or -1
         var wheel = event.deltaY < 0 ?
            1 : -1;
         
         // Compute zoom factor
         var scale = Math.exp(
            wheel * UserInput.ZOOM_INTENSITY
         );
      
         
         var matrix = new Matrix();
         
         matrix.translateSelf(
            point.x, point.y, 0
         );
      
         matrix.scaleSelf(
            scale, scale, 1
         );
      
         matrix.translateSelf(
            -point.x, -point.y, 0
         );
     
         // Start the timer for mouse
         // transform 
         endTransformTimer =
            window.setTimeout(
               onendtransform,
               UserInput.TRANSFORM_TIMEOUT
            );

         // Fire the transform event
         userInput.transform(
            matrix
         ).catch(
            (error) => userInput.handleError(error)
         );
      
      
      }
      
      
      // a long press has occurred
      function onlongpress(event) {
      
         // reset the timer
         longPressTimer = null;
         
         // cancel the selected line
         userInput._drawing = false;
 
         userInput.longPress(longPressPoint)
         .catch(
            (error) => userInput.handleError(error)
         );
        
      }
 
      // Transform has ended
      function onendtransform(event) {
         endTransformTimer = null;

         userInput.endTransform().catch(
            error => userInput.handleError(error)
         );

      }
      
      // Clear the timeout for long
      // press.
      function clearLongPressTimer(reason){
         
         if (longPressTimer) {
            window.clearTimeout(
               longPressTimer
            );
            longPressTimer = null;
         }
         
      }
      
      // Clear the timeout for end
      // transform
      function clearTransformTimer(){
         
         if (endTransformTimer) {
            window.clearTimeout(
               endTransformTimer
            );
            endTransformTimer  = null;
         }
         
      }

      function getPoint(event, touchIndex) {

         var x, y;
   
         if (event.touches) {
            var touch =
               event.touches[
                  touchIndex
               ];
               
            x = touch.clientX,
            y = touch.clientY
         }
         else {
            x = event.clientX,
            y = event.clientY;
         }
   
         return new Point(
            {x, y, z: 0}
         );
      }
      

      function touchTransform() {
   
         // transform and copy
         // points, transforming into
         // canvas coordinates
         var inverse = userInput.inverse;

         var canvasPoints =
            userInput._touchPoints.map(
               point => 
                  point.matrixTransform(
                     inverse
                  )
            );
            
         // points now has four points
         // Two points, from start fingers
         // and two points from moved
         // fingers
         var fromDistance =
            Point.distance(
               canvasPoints[0],
               canvasPoints[1]
            );
  
         var toDistance =
            Point.distance(
               canvasPoints[2],
               canvasPoints[3]
            );
   
         var scale =
            toDistance / fromDistance;
   
         var from =
            Point.center(
               canvasPoints[0],
               canvasPoints[1]
            );
 
         var to =
            Point.center(
               canvasPoints[2],
               canvasPoints[3]
            );
         
         // perform the actual translate
         // and scale
         
         var matrix = new Matrix();
         
         matrix.translateSelf(
            to.x, to.y, 0
         );
      
         matrix.scaleSelf(
            scale, scale, 1
         );
      
         matrix.translateSelf(
            -from.x, -from.y, 0
         );
     
         // Fire the transform event
         userInput.transform(
            matrix
         ).catch(
            (error) => userInput.handleError(error)
         );
     

      }
   
   }

   handleError(error) {
      if (error)
         if (error.stack)
            console.log(error.stack);
         else
            console.log(error);
      else
         console.log("Unkown error");
   }

   toJSON() {
      return {
         id: super.toJSON()
      }
   }

   createElement() {
      throw new Error("Should be implemented in derived class.");
   }
   
}