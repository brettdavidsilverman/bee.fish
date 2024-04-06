function Console() {

   var console = this;

   var element =
      document.createElement(
         "code"
      );
         
   element.id = "console";
      
   setStyle(element);
      
   document.body.appendChild(
      element
   );
      
   function setStyle(element) {
      
      element.style.position =
         "absolute";
      element.style.top = "0px";
      element.style.left = "0px";
      element.style.right = "0px";
      element.style.color = "red";
      element.style.wordWrap =
         "break-word";
      element.style.whiteSpace =
         "pre-wrap";
      element.style.zIndex = "-1";
   }

   
   this.log = function(obj) {
      var text =
         element.innerHTML;
      var string;
      var objects = [];
      if (obj == null)
         string = "null";
      else if (obj == undefined)
         string = "undefined";
      else if (typeof obj == "string")
         string = obj
      else if (typeof obj == "object")
         string = JSON.stringify(obj, null, "   ");
      else
         string = String(obj);
      
      text =
         Date.now() + "\t" +
         string +
         "<br/>" +
         text.substr(0, 1000);
          
      element.innerHTML = text;

   }
   
}