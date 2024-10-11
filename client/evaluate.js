function evaluate(json, parent=json)
{
   switch(typeof(json)) {
   case "string":

      var string = json
      if (string.startsWith("{") &&
          string.endsWith("}"))
      {
         
         var func = new Function(string);
         var evaluated = func.call(parent);
         
         if (evaluated == undefined)
            evaluated = string;

         return evaluated;
      }
      else
         return string;
      break;
   case "object":
      for (property in json)
      {
         var newProperty =
            evaluate(property, json);

         if (newProperty == undefined)
            newProperty = property;
            
         var newValue =
            evaluate(json[property], json);
            
         json[newProperty] =
            newValue;
            
         if (newProperty != property)
            json[property] = undefined;
      }
      return json;
      break;
    
   default:
      return json;
   }
}




function getJSON(url) {

   var response =
      fetch(
         url
      ).then(
         function (response) {
            if (response.status == 404)
               throw new Error("Not found");
            else if (response.status != 200)
               throw new Error(response.status);
               
            return response.text();
         }
      ).catch(
         function(error) {
            alert(error);
         }
      );

   return response;
}

function HTML(url, parent=document.body) {

   var element;
   getJSON(url)
   .then(
      (json) => {
         var element = createElement(JSON.parse(json), parent);
      }
   );
   
   function createElement(json, parent)
   {
      for (var tag in json) {
             
         var element =
           document.createElement(tag);

         for (attribute in json[tag])
         {
            if (attribute == "children") {
               var children = json[tag].children;
               for (var index in children) {
                  var child = children[index];
                  createElement(child, element);
               }
            }
            else {
               element[attribute] = json[tag][attribute];
            }
         }
            
         parent.appendChild(element);
      
      }
   }
   
}
