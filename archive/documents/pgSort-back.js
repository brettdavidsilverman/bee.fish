function pgSort() {

    this.depth = 0;

    this.data = new Array(null, null);

    this.addRecord = function(record) {

        // Push the tree down to 
        // match keys length
        while (record.length > this.depth) {
            this.data = new Array(this.data, null);
            this.depth++;
        }
       
        // Start the new path so its depth
        // is equal for all keys
        var path = this.data;
        var depth = this.depth;
        while (depth > record.length) {
            path = path[0];
            depth--;
        }

        // Read a bit at a time from the key
        // and add its path to the tree
        while ((bit = record.nextBit()) != null) {
            if (path[bit] == null) {
                path[bit] = new Array(null, null);
            }
            path = path[bit];
        }
        path.value = record.value;
    }

    this.traverse = function(callback) {
        traverse(this.data, callback);
    }

    function traverse(path, callback) {
        // Output the value
        if (path.value != null) 
            callback(path.value);

        // Traverse low
        if (path[0] != null)
            traverse(path[0], callback);

        // Traverse high
        if (path[1] != null)
            traverse(path[1], callback);
    }

    return this;
}

Number.prototype.toBitArray = function () {
    // convert number to string of base 2
    var string = this.toString(2);

    // convert string to bit array
    var bitArray = new Array();
    for (bit in string) {
        var number = parseInt(string[bit]);
        bitArray.push(number);
    }
    return bitArray;
}

function NumberRecord(value) {
    this.value = value;
    var bits = value;
    var length = 0;
    while (bits > 0) {
        length++;
        bits = bits >> 1;
    }
    this.length = length;
    this.nextBitPosition = this.length - 1;
}

NumberRecord.prototype = new Object();

NumberRecord.prototype.nextBit = function () {
    if (this.nextBitPosition < 0)
        return null;

    var nextBit = (this.value >> this.nextBitPosition) & 1;

    this.nextBitPosition--;

    return nextBit;
}

function StringRecord(value) {
    this.value = value;
    this.nextCharacterPosition = 0;
    this.nextCharacter = value.charCodeAt();
    this.nextBitPosition = StringRecord.bitsPerCharacter - 1;
    this.length = this.value.lengh * StringRecord.bitsPerCharacter;

    return this;
}

StringRecord.bitsPerCharacter = 32;

StringRecord.prototype = new Object();

StringRecord.prototype.nextBit = function() {

    if (this.nextCharacterPosition == this.value.length)
        // All bits have been read
        return null;

    // get the next bit by masking
    // the next character
    var nextBit = (this.nextCharacter >> this.nextBitPosition) & 1;

    if (this.nextBitPosition == 0) {
        // Shift to next character
        this.nextBitPosition = StringRecord.bitsPerCharacter - 1;
        this.nextCharacterPosition++;
        this.nextCharacter = this.value.charCodeAt(this.nextCharacterPosition);
    }
    else
        // Shift to next significant bit
        this.nextBitPosition--;

    // return the next bit
    return nextBit;
}
