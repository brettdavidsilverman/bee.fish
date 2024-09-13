function evaluate(json, parent)
{
   switch(typeof(json)) {
   case "string":
      var string = json;
      if (string.startsWith("_{") &&
          string.endsWith("}"))
      {
         var func = new Function(string.substr(1));
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

function HTML(json) {

   var element;
   
   for (var tag in json) {
      var element =
         document.createElement(tag);

      for (attribute in json[tag])
         element[attribute] = json[tag][attribute];
         
      document.body.appendChild(element);
      
   }
   
}
