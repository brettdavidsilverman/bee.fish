export class Processor extends
      AudioWorkletProcessor
{
    constructor() {
        super();
        this.index = 0;
        this.port.onmessage =
        (event) => {
            Object.assign(this, event.data);
        };
    }
    
    // Override this in derived class
    /*
    f(timer, inputs)
    {
    }
*/
    process(inputs, outputs, parameters) {

        const output = outputs[0];
        
        for (let channel = 0;
             channel < output.length; 
             ++channel
            ) 
        {

            const outputChannel = 
                output[channel];

            for (let i = 0; 
                 i < outputChannel.length;
                 ++i
                ) 
            {
                
                var _inputs = [];
                for (var j = 0; 
                     j < inputs.length;
                     ++j)
                {

                    for (var k = 0;
                         k < inputs[j].length;
                         ++k)
                    {
                        _inputs[j] = inputs[j][k][i];
                    }

                }
                outputChannel[i] = 
                    this.f(this.index++, _inputs);
            }

        }
        return true;
    }
}

