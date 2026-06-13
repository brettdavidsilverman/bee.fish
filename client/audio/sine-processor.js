// sine-processor.js
class SineProcessor extends
    AudioWorkletProcessor
{
    constructor() {
        super();
        this.phase = 0;
        this.frequency = 100; // hz
        this.t = 0;
        var self = this;
        this.port.onmessage =
        (event) => {
            Object.assign(this, event.data);
        };
    }

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
                outputChannel[i] =
                    Math.sin(this.phase);

                this.phase += 
                    (2 * Math.PI * this.frequency)
                    / this.sampleRate;
            }
        }
        return true;
    }
    
    
}
registerProcessor("processor", SineProcessor);
