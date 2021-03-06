// http://brett.sourceforge.net/Encoding/power%20Encoding.js
function PowerEncoding()
{
	base = 2;
	
	Number.prototype.power = function() {
		return this.toString(base).length - 1;
	}
	
	Number.prototype.remainder = function() {
		return this - Math.pow(base, this.power());
	}

	Number.prototype.add = function(remainder) {
		return Math.pow(base, this) + remainder;
	}
	
	Number.prototype.encode = encode;
	
	function encode() {

		if (this == 0)
			return Zero;

		return new Encoding(this.power().encode(), this.remainder().encode());
	}
	
	function parse(value) {
        bit = value.string[value.offset++];
        nextBit = value.string[value.offset];

        if (bit == "1") {
            if (nextBit == "0") {
                value.offset++;
                return new Encoding(Zero, Zero);
            }
            return new Encoding(parse(value), parse(value));
        }
          
	    return Zero;

	}

	this.parse = function(string) {
        var value;
        if (typeof(string) == "string")
            value = { string : string, offset : 0 }
        else if (string.isBitArray)
            value = { string : string.join(""), offset : 0 }
        else
            value = string;

        return parse(value);
    }

	function Encoding(power, remainder) {

		this.power = power;
		this.remainder = remainder;

		this.toString = function () {
            if (this.power == Zero && this.remainder == Zero)
                return "10";
            return "1" + this.power.toString() + this.remainder.toString();
		}
		
		this.decode = function() {
			return this.power.decode().add(this.remainder.decode());
		}

        this.toBitArray = function() {
            var string = this.toString();
            var bitArray = new BitArray();
            for (var position = 0; position < string.length; position++) {
                if (string[position] == "0")
                    bitArray.push(0);
                else
                    bitArray.push(1);
            }
            return bitArray;
        }

	    this.json = function () {
            return JSON.stringify(this);
        }

		return this;
	}
		
	var Zero = {
		decode : function() {
            return 0;
        },
        toString : function() {
            return "0";
        }, 
        toBitArray : function() {
            return [0];
        }
	}

	return this;
}

encoding = new PowerEncoding();
/*
alert((3).encode());
alert((3).encode().decode());
alert(encoding.parse((3).encode().toString()));
*/
/*
var x = 0;
var totalEncoded = "";
var total = 0;
for (x = 0; x < Math.pow(2, 8); x++) {
    totalEncoded += x.encode().toString();
    total += 8;
    document.write(x + ":" + parseInt(x.encode().toString(), 2));
    document.writeln("</br>");
    if (x != encoding.parse(x.encode().toString()).decode())
        alert(x);
}
x = 0;
encoded = { string : totalEncoded, offset : 0 };
while (encoded.offset < encoded.string.length) {
    y = encoding.parse(encoded).decode();
    if (x != y)
        alert([x, y]);
    x++;
    //document.writeln("</br>");
}
document.write(totalEncoded.length);
document.write("/" + total);
*/
//alert((0).encode());
//alert((1).encode());
//alert((2).encode());
//alert((4).encode().decode());

//alert(number.encode().toString());
//alert(number.encode().decode());
//alert(context.toChar().charCodeAt());
/*
byteLength = 32;
maxPower = 8;
length = 0;
for (i = 0; i < Math.pow(2, maxPower); i++) {
	if (i != i.encode().decode()) {
		document.write("i: " + i.toString());
			//	" : " + JSON.stringify(i.encode()) + 
		//document.write(" : " + JSON.stringify(i.encode())),
		document.write(" : " + i.encode().toString());
		//document.write(" : " + encoding.parse(i.encode().toString()).toString()) +
			//	" : " + encoding.parse(i.encode().toString()).toString() +
		document.write(" : " + encoding.parse(i.encode().toString()).toString());
		document.write("</br>");
	}
}
document.write((Math.pow(2, maxPower)*byteLength - length)/Math.pow(2,maxPower)*byteLength);
document.write("<br>");
document.write(Math.pow(2, maxPower));
*/
