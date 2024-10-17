function evaluate(json, parent=json)
{
   switch(typeof(json)) {
   case "string":

      var string = json
      if (string.startsWith("{") &&
          string.endsWith("}"))
      {
          
         var f = new Function(string);
         var evaluated = f.call(parent);

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




function fetchJSON(url) {

  // url = document.location.origin + 
  //      "/" + url;

   var promise =
      fetch("https://bee.fish/" + url).
      then(
         function (response) {
            if (response.status == "404")
               return undefined;
            else if (response.status != "200")
               throw response.statusText;
               
            return response.json();
         }
      ).
      catch(
         function(error) {
            Error(error + "\r\n" + url, fetchJSON);
         }
      );
      
   return promise;

}

function postJSON(url, json) {
   var promise =
      fetch(
         "https://bee.fish/" + url,
         {
            method: "POST",
            body: json
         }
      ).then(
         function (response) {
            return response.text();
         }
      ).then(
         function (json) {
            alert(json);
         }
      ).catch(
         function(error) {
            Error(error + "\r\n" + url, fetchJSON);
         }
      );

   return promise;
}


function HTML(url, parent=document.body) {
   

   if (typeof url == "object")
   {
      var json = evaluate(url);
      var element = createElement(json, parent);
      return element;
   }
   else
   {
      url = evaluate(url);
      
      var promise =
         fetchJSON(url).
         then(
            function(json)
            {
                json = evaluate(json);
                
                var element = createElement(json, parent);
                return element;
            }
         ).
         catch(
            function (error) {
               Error(error);
            }
         );
         
      return promise;
   }
   
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