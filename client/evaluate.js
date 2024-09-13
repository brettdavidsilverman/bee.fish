


function postJSON(url, json) {
   var response =
      fetch(
         url,
         {
            method: "POST",
            body: json
         }
      ).then(
         function (response) {
            return response.json();
         }
      ).then(
         function (json) {
            return json;
         }
      ).catch(
         function(error) {
            alert("Error " + error);
         }
      );

   return response;
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
