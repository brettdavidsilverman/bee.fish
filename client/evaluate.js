function evaluate(json, parent=json)
{

   switch(typeof(json)) {
   case "string":

      var string = json;
 
      if (string.startsWith("{") &&
          string.endsWith("}"))
      {
         var value =
            string.substring(1, string.length - 1);
         
         var f =
            new Function(
               "return (" + 
                   value +
               ")"
            )
         
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
            
         if (property != newProperty)
            json[property] = undefined;
      }
      return json;
      break;
    
   default:
      return json;
   }
}




function fetchJSON(url) {

   var promise =
      fetch("https://bee.fish/" + url).
      then(
         function (response) {
            return response.text();
         }
      ).
      then(
         function (text) {
            return JSON.parse(text);
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

function functionsToJSON(json)
{
   if (typeof json == "function")
      return "{" + json + "}";

   if (typeof json != "object")
      return json;
      
   for (var property in json) {
      json[property] =
         functionsToJSON(
            json[property]
         );
   }
   
   return json;
   
}

function JSONToFunctions(json) {
   
   var strings = [];
   ObjectToString(json, strings);
   var string = strings.join("");
   return string;
   
   function ObjectToString(object, strings)
   {
      if (typeof object == "string") {
         var string = object;
         if (string.startsWith("{function") &&
             string.endsWith("}"))
         {
            var f = new Function(
               "return " +
                  string.substring(
                     1, string.length - 1
                  )
            );
            string = f().toString();
            strings.push("\r\n" + string + "\r\n");
         }
         else
            strings.push(JSON.stringify(string));
      }
      else if (typeof object != "object")
         strings.push(JSON.stringify(object));
      else {
      
         strings.push("{");
         var count =
            Object.keys(object).length;

         var i = 0;
         
         for (var property in object)
         {
            strings.push(
               JSON.stringify(property)
            );
            strings.push(":");
            ObjectToString(
               object[property],
               strings
            );
            if (++i < count)
               strings.push(",\r\n");
         }
         
         strings.push("\r\n}")
         
      }
   }
}
   
   
function formatJSON(jsonText){
    
   var f = new Function("return (" + jsonText + ");");
   
   var json = f();
   
   json = functionsToJSON(json);
   
   var formatted =
      JSON.stringify(
         json,
         null,
         "   "
      );

   return formatted;
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