class ChildPointer extends Pointer {
    parent;

    constructor(input) {
        super(getPointer(input));

        this.parent = input.parent;

        function getPointer(input) {
            if (input && input.pointer)
                return input.pointer;
            else if (input && input.key) 
                return {key : input.key};
            else
                return input;
        }
    }

    toJSON() {
        return this.key
    }

    async load() {
        var item = await super.load();
        if (item)
            item.parent = this.parent;
        return item;
    }
}