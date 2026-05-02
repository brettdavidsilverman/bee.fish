// my-processor.js
class SumProcessor extends AudioWorkletProcessor {
    process(inputs, outputs, parameters) {
        const input1 = inputs[0]; // First input
        const input2 = inputs[1]; // Second input
        const output = outputs[0]; // Output to connect to

        // Assuming both inputs are stereo (2 channels)
        // iterate over each channel (0 and 1)
        for (let channel = 0; channel < output.length; channel++) {
            const outputChannel = output[channel];
            const input1Channel = input1[channel];
            const input2Channel = input2[channel];

            // If inputs exist, add them
            if (input1Channel && input2Channel) {
                for (let i = 0; i < outputChannel.length; i++) {
                    // SUMMATION HAPPENS HERE
                    outputChannel[i] = input1Channel[i] + input2Channel[i];
                }
            } else if (input1Channel) {
                outputChannel.set(input1Channel);
            } else if (input2Channel) {
                outputChannel.set(input2Channel);
            }
        }

        return true; // Keep the processor alive
    }
}

registerProcessor('sum-processor', SumProcessor);
