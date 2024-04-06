class Point extends Id {

   constructor(input) {
      super(input);

      const initial = {
         x: 0,
         y: 0,
         z: 0,
         w: 0
      }

      if (input == undefined) {
         input = initial;
      }

      this.x = input.x;
      this.y = input.y;
      this.z = input.z;
      this.w = input.w;

   }

   toJSON() {
      return {
         x: this.x,
         y: this.y,
         z: this.z,
         w: this.w
      }
   }
   
   toString()
   {
      return JSON.stringify(
         {
            x: this.x,
            y: this.y,
            z: this.z
         }
      );
   }
   
   matrixTransform(matrix)
   {
      var point = new DOMPoint(this.x, this.y, this.z, this.w);
      var transformed = point.matrixTransform(matrix);
      return new Point(transformed);
   }
      
   static distance(p1, p2) {
      var dx = p2.x - p1.x;
      var dy = p2.y - p1.y;
   
      return Math.sqrt(
         dx * dx + dy * dy
      );
   }
   
   static center(p1, p2) {
      return new Point(
         {
            x: (p1.x + p2.x) / 2,
            y: (p1.y + p2.y) / 2
         }
      );
   }

   static max(p1, p2) {
      return new Point(
         {
            x: Math.max(
               p1.x,
               p2.x
            ),
            y: Math.max(
               p1.y,
               p2.y
            )
        }
      );
   }

   static min(p1, p2) {
      return new Point(
         {
            x: Math.min(
               p1.x,
               p2.x
            ),
            y: Math.min(
               p1.y,
               p2.y
            )
         }
      );
   }

   copy() {
      var point = this.toJSON();
      var copy = new Point(point);
      return copy;
   }
   
}
