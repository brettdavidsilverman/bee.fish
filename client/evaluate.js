function evaluate(json, parent=json)
{

   if (Array.isArray(json)) {
      var array = json;
      for (var i in array) {
         array[i] = evaluate(array[i], parent);
      }
      return array;
   }
   
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

   var status;
   
   var promise =
      fetch(url).
      then(
         function (response) {
            status = response.status;
            return response.json();
         }
      ).
      then(
         function (json) {
            if (status != "200") {
               if (json.error)
                  throw json.error;
               throw json;
            }
            return json;
         }
      );
      
   return promise;

}

function postJSON(url, json) {
   var status;

   var parameters = {
      method: "POST",
      body: JSON.stringify(json)
   }
  
   var promise =
      fetch(
         url,
         parameters
      ).
      then(
         function (response) {
            status = response.status;
            return response.json();
         }
      ).
      then(
         function (json) {
             
            if (status != "200")
               throw json;
               
            return json;
         }
      );

   return promise;
}

function hideFunctions(json)
{

   if (Array.isArray(json)) {
      var array = json;
      for (var i = 0; i < array.length; i++) {
         array[i] = hideFunctions(array[i]);
      }
      return array;
   }
   
   if (typeof json == "function")
      return "{" + json + "}";

   if (typeof json != "object")
      return json;
     
   for (var property in json) {
      
      
      json[property] =
         hideFunctions(
            json[property]
         );
   }
   
   return json;
   
}

function displayFunctions(json) {
 
   const tab = "   ";
   var strings = [];
   objectToString(json, strings);
   var string = strings.join("");
   
   return string;
   
   function objectToString(json, strings, tabs = 0)
   {
      
      
      if (Array.isArray(json)) {
         strings.push(tab.repeat(tabs));
         strings.push("[\r\n");
         var array = json;
         for (var i = 0; i < array.length; i++) {
            strings.push(tab.repeat(tabs + 1));
            objectToString(array[i], strings, tabs + 1);
            if (i < array.length - 1)
               strings.push(",");
            strings.push("\r\n"); 
         }
         strings.push(tab.repeat(tabs));
         strings.push("]\r\n");
         return
      }
   
      if (typeof json == "string") {
         var string = json;
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
      else if (typeof json != "object")
         strings.push(JSON.stringify(json));
      else {
         var object = json;
         //strings.push(tab.repeat(tabs));
         strings.push("{\r\n");
         
         var count =
            Object.keys(object).length;

         var i = 0;
         
         for (var property in object)
         {
            strings.push(tab.repeat(tabs + 1));
            strings.push(
               JSON.stringify(property)
            );
            strings.push(": ");
            objectToString(
               object[property],
               strings,
               tabs + 1
            );
            if (++i < count) 
               strings.push(",");
               
            strings.push("\r\n");
         }
         
         strings.push(tab.repeat(tabs));
         strings.push("}")
         
      }
   }
}
   
   
function formatJSON(jsonText){
    
   var f;
   
   f = new Function("return " + jsonText);
   
   var json = f();
   
   json = hideFunctions(json);
   
   var formatted =
      JSON.stringify(
         json,
         null,
         "   "
      );

   return json;
}

function HTML(url, parent=document.body) {
   

   if (typeof url == "object")
   {
      var json = evaluate(url);
      createElement(json, parent);
      return Promise.resolve();
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
      if (Array.isArray(json))
      {
         for (var index in json)
         {
            var obj = json[index];
            createElement(obj, parent);
         }
         
         return;
      }
      
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
               var value = json[tag][attribute];
               if (value)
                  element[attribute] = value;
            }
         }
         
         parent.appendChild(element);
      }
            
      
      
   }
   
}