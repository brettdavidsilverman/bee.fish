// sine-processor.js
class SineProcessor extends
    AudioWorkletProcessor
{
    constructor() {
        super();
        this.phase = 0;
        this.frequency = 440;
        this.sampleRate = 44100;
        this.playing = false;
        var _this = this;
        
        this.port.onmessage =
            (event) => {
                var data = event.data;
                
                if (data == "start")
                    _this.start();
                else if (data == "stop")
                    _this.stop();
                else if (typeof data == "object")
                {
                    if (data.sampleRate != undefined)
                        _this.sampleRate =
                            data.sampleRate;
                            
                    if (data.frequency != undefined)
                        _this.frequency =
                            data.frequency;
                }
                
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
                if (this.playing)
                {
                    outputChannel[i] =
                        Math.sin(this.phase);
                }
                else
                    outputChannel[i] = 0;
                    
                this.phase += 
                    (2 * Math.PI * this.frequency)
                    / this.sampleRate;
            }
        }
        return true;
    }
    
    start() {
        this.playing = true;
    }
    
    stop() {
        this.playing = false;
    }
    
}
registerProcessor('sine-processor', SineProcessor);
