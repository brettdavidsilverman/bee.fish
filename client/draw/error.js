var lastApp;
window.addEventListener("error", onerror);
function onerror() {
   if (lastApp)
   {
      lastApp.promptStatement();
   }
}