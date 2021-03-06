function Store(parent) {

    this.parent = parent;
    this.low = undefined;
    this.high = undefined;

    return this;
}

Store.prototype = {

    store : function(bitArray) {

        if (bitArray.length == 0)
            return this;

        var bit = bitArray.shift();
        if (bit == 0) {
            if (this.low == undefined)
                this.low = new Store(this);
            return this.low.store(bitArray);
        }
        else {
            if (this.high == undefined)
                this.high = new Store(this);
            return this.high.store(bitArray);
        }

        return this;
    },

    toString : function() {

        var string = "";

        if (this.low == undefined)
            string += "0";
        else
            string += this.low.toString();

        if (this.high == undefined)
            string += "0";
        else
            string += this.high.toString();

        return string + "1";

    },

    first : function() {

        if (this.low != undefined)
            return this.low.first();

        if (this.high != undefined)
            return this.high.first();

        return this;
    },

    last : function() {

        if (this.high != undefined)
            return this.high.last();

        if (this.low != undefined)
            return this.low.last();

        return this;
    },

    next : function() {
        if (this.high != undefined)
            return this.high.first();
        return this.parent.next();
    },

    previous : function() {
        if (this.low != undefined)
            return this.low.last();
        return this.parent.previous();
    },

    toBitArray : function() {
        var bitArray;
        if (this.parent == undefined)
            bitArray = new Array();
        else {
            bitArray = this.parent.toBitArray();
            var bit;
            if (this.parent.low == this)
                bit = 0;
            else
                bit = 1;
            bitArray.push(bit);
        }

        return bitArray;
    }
}

Store.parse = function(string, parent) {

    if (string.pop() == "0")
        return undefined;

    var store = new Store(parent);

    store.high = Store.parse(string, store);
    store.low = Store.parse(string, store);

    return store;
}