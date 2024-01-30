onmessage = function(event) {
   var id = event.data;
   
   var url = new URL(
      "http://bee.fish/db/db.bin"
   );
   url.searchParams.set("id", id);
   const response = await fetch(url);
   /*
   postMessage(url);
   var request = new XMLHttpRequest();
   request.open("GET", url, false);
   request.responseType = "text";
   request.send();
   */
   postMessage(response);
   
}