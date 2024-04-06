async function example() {

    const encode = (e => e.encode.bind(e))(new TextEncoder);

    let {publicKey: publicKey, privateKey: privateKey} =
        await crypto.subtle.generateKey(
            {
                name: "RSA-OAEP",
                // Consider using a 4096-bit key for systems that require long-term security
                modulusLength: 2048,
                publicExponent: new Uint8Array([1, 0, 1]),
                hash: "SHA-256",
            },
            true,
            ["encrypt", "decrypt"]
        );
  
    let cipherText =
        await crypto.subtle.encrypt(
            {
                name: "RSA-OAEP"
            },
            publicKey,
            encode("Hello World")
        );
        
    // same on the server

    let decrypted = 
        await window.crypto.subtle.decrypt(
            {
                name: "RSA-OAEP"
            },
            privateKey,
            cipherText
        );

    var decoder = new TextDecoder("utf-8");
    var text = decoder.decode(decrypted);

    alert(text);

}

example();
