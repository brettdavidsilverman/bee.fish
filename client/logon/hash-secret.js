importScripts("/client/logon/sha512.js");

onmessage = async function(e) {
   const sha = new jsSHA("SHA-512", "ARRAYBUFFER");
      
   var file = e.data;
   
   var bytes = await file.arrayBuffer();
   
   sha.update(bytes);
      
   const hash = sha.getHash("HEX");

   postMessage(hash);
}