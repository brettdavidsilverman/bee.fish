window.getPixelsPerMillimeter =
function() {

   if ( window._pixelsPerMillimeter )
      return window._pixelsPerMillimeter;
      
   var element =
      document.createElement("div");
      
   element.style.position = "absolute";
   element.style.width = "100mm";
   element.style.height = "100mm";
   
   document.body.appendChild(
      element
   );
   
   var rect =
      element.getBoundingClientRect();
  
   document.body.removeChild(element);
   
   var pixelsPerMillimeter = new Point( {
      x: rect.width / 100,
      y: rect.height / 100
   } );
 
   window._pixelsPerMillimeter =
      pixelsPerMillimeter;
      
   return window._pixelsPerMillimeter;
}

window.addEventListener("resize",
   function() {
      delete window._pixelsPerMillimeter;
   }
);

Object.defineProperty(
   window,
   "selection",
   {
      get: function() {
         return canvas.selection;
      },
      set: function(value) {
         canvas.selection = value;
      }
   }
);


