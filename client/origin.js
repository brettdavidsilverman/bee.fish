var header = document.getElementById("h1");
var title = document.getElementById("title");

var origin =
   punycode.toUnicode(
      window.location.hostname
   );

if (header)
   header.innerText = "Process " + origin;
   
if (title)
   title.innerText = origin;
