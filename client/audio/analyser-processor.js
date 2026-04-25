// analyser-processor.js
class AnalyserProcessor extends AudioWorkletProcessor {
    process(inputs, outputs, parameters) {
        const input = inputs[0];

        // Check if input is active
        if (input.length > 0 && input[0].length > 0) {
            // Calculate RMS (Volume)
            let sum = 0;
            for (let i = 0; i < input[0].length; i++) {
                sum += input[0][i] * input[0][i];
            }
            const rms = Math.sqrt(sum / input[0].length);

            // Send data to main thread
            this.port.postMessage({ volume: rms });
        }

        return true; // Keep processor alive
    }
}

registerProcessor('analyser-processor', AnalyserProcessor);