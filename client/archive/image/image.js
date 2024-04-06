HTMLImageElement.fromStorage =
function(json, memory) {
   var width = json.width;
   var height = json.height;
   var image = new Image(width, height);
   Object.assign(image, json);
   return image;
}

Image.prototype.toShorthand =
function(shorthand) {

   var json = {
      width:  this.width,
      height: this.height,
      src:    this.src
   }

   Object.assign(json, this);

   return json.toShorthand(shorthand);
}
