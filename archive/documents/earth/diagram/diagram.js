function Diagram(diagrams) {

    this.name = "Diagram";
    this.oldCoordinates = new Array();
    this.subdiagrams = new Subdiagrams(this, diagrams);

    this.popCoordinates = function() {
        var coordinates = this.coordinates;
        this.coordinates = this.oldCoordinates.pop();
        return coordinates;
    }

    this.pushCoordinates = function(coordinates) {
        this.oldCoordinates.push(this.coordinates);
        this.coordinates = coordinates;
    }

    this.javascript = function() {
        var javascript = new Object();
        javascript.text = "";
        javascript.writeln = function(line) {
            this.text += line += "\r\n";
        }
        for (var subdiagram in this.subDiagrams) {
            var subdiagram = new this.subdiagrams[subdiagram]();
            javascript.writeln(subdiagram.javascript());
        }
        return javascript.text;
    }

    this.translateAndScale = function(coordinates) {
        if (this.parent == undefined)
            return coordinates;
        var scale = this.totalScale();
        var parentCoordinates = this.parent.translateAndScale(this.coordinates());
        return {
            x : (parentCoordinates.x + coordinates.x * scale),
            y : (parentCoordinates.y + coordinates.y * scale)
        }
    }

    this.coordinates = function() {
        if (this.parent == undefined)
            return {x : 0, y : 0}
        return {
            x : this.parent.width() / 2 - this.width() * this.scale() / 2,
            y : 0
        }
    }
    
    

    this.width = function() {
        return canvas.width;
    }

    this.height = function() {
        return canvas.height;
    }

    this.scale = function() {
        if (this.parent == undefined)
            return 1;
        else {
            var scaleY = this.parent.height() / this.height();
            var scaleX = this.parent.width() / this.width();
            var scale = (scaleY < scaleX) ? scaleY : scaleX;
            return scale;
        }
    }

    this.totalScale = function() {
        if (this.parent != undefined)
            return this.scale() * this.parent.totalScale();
        else
            return this.scale();
    }
 
    function onMouseDown(event) {
        event.preventDefault();
        var coordinates = {
            x : event.clientX,
            y : event.clientY
        }
        diagram.mouseDown(coordinates);    
    }

    function onMouseMove(event) {
        event.preventDefault();
        var coordinates = {
            x : event.clientX,
            y : event.clientY
        }
        diagram.mouseMove(coordinates);    
    }

    function onMouseUp(event) {
        event.preventDefault();
        diagram.mouseUp();    
    }

    function onTouchStart(event) {
        event.preventDefault();
        var touch = event.touches[0];
        var coordinates = {
            x : touch.clientX,
            y : touch.clientY
        }
        diagram.mouseDown(coordinates);
    }

    function onTouchMove(event) {
        event.preventDefault();
        var touch = event.touches[0];
        var coordinates = {
            x : touch.clientX,
            y : touch.clientY
        }
        diagram.mouseMove(coordinates);
    }

    function onTouchEnd(event) {
        event.preventDefault();
        diagram.mouseUp();
        //alert("touchend");
    }

    function onResize(event) {
        diagram.remove();
        diagram.create(diagram.element);
        diagram.diagram();
    }

    this.resize = function() {
        onResize();
    }

    this.create = function(element) {
        this.element =element;
        var parentElement = this.element.parentElement;
        canvas = document.createElement("canvas");
        canvas.width = parentElement.clientWidth;
        canvas.height = parentElement.clientHeight;
        if ("ontouchstart" in canvas) {
            canvas.ontouchstart = onTouchStart;
            canvas.ontouchmove = onTouchMove;
            canvas.ontouchend = onTouchEnd;
        }
        else {
            canvas.onmousedown = onMouseDown;
            canvas.onmousemove = onMouseMove;
            canvas.onmouseup = onMouseUp;
        }
        parentElement.insertBefore(canvas, element);
        window.addEventListener("resize", onResize);
    }

    this.remove = function() {
        var parentElement = this.element.parentElement;
        parentElement.removeChild(canvas);
        window.removeEventListener("resize", onResize);
    }

    var diagram = this;
    var canvas;
    this.canvas = function() {
        if (this.parent != undefined)
            return this.parent.canvas();
        return canvas;
    }
    this.context = function() {
        return this.canvas().getContext("2d");
    }

    this.diagram = function() {
        var context = this.context();
        context.strokeStyle="#000000";
        context.fillStyle = "#FFFFFF";
        //context.fillRect(0, 0, this.width(), this.height());
        if (this.subdiagrams.length > 0) {
            this.subdiagrams.diagram();
        }
    }

    this.mouseDown = function(coordinates) {
        this.hit = this.subdiagrams.hitTest(coordinates);
        if (this.hit != undefined) {
            this.hit.lastCoordinates = this.hit.coordinates();
            this.hit.moveCoordinates = coordinates;
            this.hit.firstCoordinates = coordinates;
            this.hit.originalCoordinates = this.hit.coordinates();
            this.hit.pushCoordinates(lastCoordinates);
            this.hit.pushCoordinates(moveCoordinates);
         }
    }

    this.mouseMove = function(coordinates) {

        if (this.hit != undefined) {
            this.hit.moveCoordinates = coordinates;
        }
    }

    this.mouseUp = function() {
        if (this.hit != undefined) {
            this.hit.popCoordinates();
            delete this.hit;
            this.diagram();
        }
    }

    function lastCoordinates() {
        return this.lastCoordinates;
    }

    function moveCoordinates() {
        var scale = this.parent.totalScale();
        return  {
            x : (this.moveCoordinates.x - this.firstCoordinates.x) / scale + this.originalCoordinates.x,
            y : (this.moveCoordinates.y - this.firstCoordinates.y) / scale + this.originalCoordinates.y,
        }
    }
/*
    this.toClientCoordinates = function(parentCoordinates) {
        var coordinates = this.coordinates();
        var scale = this.scale();
        return {
            x : (parentCoordinates.x - coordinates.x) / this.scale,
            y : (parentCoordinates.y - coordinates.y) / this.scale
        }
    } 
*/
}

