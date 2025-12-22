class Id {
 
    static version = "id-v0.1.0-client";
    
    // Creates a value that is
    // guaranteed to be unique
    // from this device.
    // The value is milliseconds (time)
    // and an increment (increment)
    // for objects created close
    // in time to each other.
    // The increment is reset when
    // the milliseconds ticks over
    constructor(input) {
        
        
        this.milliseconds = undefined;
        this.increment  = undefined;
        this.key  = undefined;
        this.name = undefined;

        Object.assign(this, input);

        if ( this.key &&
                 ( this.milliseconds == undefined ||
                    this.increment  == undefined ||
                    this.name == undefined ) )
            this._decodeKey();
        else if ( this.milliseconds == undefined ||
                     this.increment == undefined )
            this._createTimestamp();
        
        if ( this.name == undefined )
        {
            this.name = this.constructor.name;
        }
        
        if ( this.key == undefined )
        {
            Object.defineProperty(
                this, 
                "key",
                {
                    get() {
                        var key = this._encodeKey();
                        return key;
                    },
                    set(key) {
                        Object.defineProperty(
                            self,
                            "key",
                            {
                                value: key,
                                enumerable: true,
                                writable: true,
                                configurable: true
                            }
                        );
                    },
                    enumerable: true,
                    configurable: true,

                }
            );
        }
        

    }
    
    static fromKey(key) {    
        return new Id({key});
    }
    
    _createTimestamp() {
        // create a new timestamp
        var time = Date.now();
        var milliseconds = time;
        
        if (milliseconds <= Id.milliseconds)
            ++Id.increment;
        else
            Id.increment = 0;
                
        Id.milliseconds = milliseconds;

        this.milliseconds = milliseconds;
        this.increment = Id.increment;

    }

    _encodeKey()
    {

        if ( this.milliseconds == undefined ||
              this.increment == undefined )
            this._createTimestamp();
            
        var stream = new PowerEncoding();
        
        // Start bit
        stream.write("1");
        
        // encode name
        if (this.name == undefined)
            this.name = this.constructor.name;

        this.name.encode(stream);
        
        // encode milliseconds
        this.milliseconds.encode(stream);
        
        // encode incrementrement
        this.increment.encode(stream);
        
        // end bit
        stream.write("0");
        
        var data = stream.toString();
        
        var key = btoa(data);
        
        Object.defineProperty(
            this, 
            "key",
            {
                value: key,
                enumerable: true,
                configurable: true
            }
        );
        
        return key;
            
    }
        
    _decodeKey() {
    
        if (this.key == undefined)
        {
            this._encodeKey();
            return;
        }
        
        // extract the name and timestamp
        // from the key
        var data = atob(this.key);
        
        var stream = new PowerEncoding(
            data
        );
            
        CHECK(
            "Id._decodeKey start bit",
            stream.read() == "1"
        );
        
        // read the name
        this.name = String.decode(stream);

        // read the time
        this.milliseconds = Number.decode(stream);
        
        // read the incrementrement
        this.increment = Number.decode(stream);
        
        CHECK(
            "Id._decodeKey end bit",
            stream.read() == "0"
        );
        
        CHECK(
            "Id._decodeKey count",
            stream.count == 0
        );


    }
    
    toString() {
        return JSON.stringify(
            this,
            null,
            "    "
        );
    }
    
    static getType(name) {
    
        if (Id.types[name])
            return Id.types[name];
            
            
        Id.types[name] =
            createType(name);
        
        return Id.types[name];
        
        function createType(name) {
            var f = new Function(
                "return " + name + ";"
            );
            return f();
        }
        
    }
    
    toJSON() {
        return {
            name: this.name,
            milliseconds: this.milliseconds,
            increment: this.increment
        }
    }
 
    fromJSON(json) {
        return new Id(json);
    }    

    save() {
        var value = JSON.stringify(this, null, "    ");

        
        var self = this;
        var promise = storage.setItem(
            this,
            value
        )
        .then(
            function() {
                return self.key;
            }
        )

        return promise;

    }
    
    remove () {
        this.release();
        var id = this;
        var key = id.key;
        var promise = 
            storage.removeItem(
                id
            ).then(
                function() {
                    return key;
                }
            );
        return promise;
    }

    load(input) {

        var self = this;
console.log("ID load: " + this.key);
        var promise = storage.getItem(this)
            .then(
                function(value) {

                    return JSON.parse(value);
                }
            )
            .then(
                function(json) {
            
                    if (json == null)
                        return null;

                    if (input == undefined)
                        input  = {};


                    if (typeof json === "object") {
                        Object.assign(input, json);
                    }
                    else
                        input = json;
                        
                    var Type = Id.getType(self.name);
                    var object = new Type(input);
                    return object;                    
                }
            );

        return promise;

    }
    
    equals(id)
    {
        var bool =
            (
                (this.name == id.name) &&
                (this.milliseconds == id.milliseconds) &&
                (this.increment == id.increment)
              
            );
        return bool;
    }

    release() {
    }
    
}

Id.milliseconds = Date.now();
Id.increment = 0;
Id.types = new Map();