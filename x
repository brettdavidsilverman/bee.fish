<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="utf-8"/>
        <meta name="viewport" content="width=device-width, initial-scale=1"/>
        <title id="title">bee.fish</title>
        <script src="/head.js"></script>
        <script src="/client/stream/stream.js"></script>
        <script src="/client/power-encoding/power-encoding.js"></script>
        <script src="/client/id/id.js"></script>
        <script src="/client/console/console.js"></script>
        <script src="/client/evaluate.js"></script>
        <script src="/client/logon/authentication.js"></script>
        <script src="/client/punycode.js"></script>
        <script src="/client/coordinates/script.js"></script>
        <script src="/client/movable/script.js"></script>
       
        <link rel="stylesheet" type="text/css" href="/style.css"/>
        <style>
#image {
    width: 100px;
    height: 100px;
    display: none;
}

        </style>

    </head>
    <body>
        <h1 id="h1">
            <a id="a">bee.fish</a>
        </h1>
        
        <form id="download">
            <input id="input" value="my"></input>
            <button type="submit">fetch</button>
        </form>
        
        <form id="upload">
            <textarea id="editor">
            </textarea>
            <button type="submit">save</button>
        </form>

        
        

        <script type="module">

const download =
    document.getElementById("download");
    
const input =
    document
    .getElementById("input");
    
const result =
    document
    .getElementById("result");

const upload =
    document
    .getElementById("upload");
    
const editor =
    document
    .getElementById("editor");
    

download.onsubmit =
async (event) => {
    try{
    
        event.preventDefault();
        
        if (download.controller)
            download.controller.abort("User cancelled");
            
        download.controller = 
           new AbortController();
        
        var url = new URL(
            input.value,
            document.location.origin
        );
        
        var response = await
            fetch(
                url,
                {
                    mode: "cors",
                    method: "GET",
                    credentials: "include",
                    signal: download.controller.signal
                }
            );

        // This checks for login
        // or error
        if (!await checkResponse(response))
            return;
            
        var text = await response.text();
        
        if (text != undefined)
            editor.value = text;
        else
            editor.value = "undefined";
        
        return false;
    }
    catch(error)
    {
        alert(error);
    }
    finally {
        download.controller = undefined;
    }
}
    
upload.onsubmit =
async (event) => {
    try{
        event.preventDefault();
        
        if (upload.controller)
            upload.controller.abort("User cancelled");
            
        upload.controller =
            new AbortController();
        
        var url = new URL(
            input.value,
            document.location.origin
        );
        
        // Remove search from url
        url = new URL(
            url.origin +
            url.pathname
        );
        
        var response = await
            fetch(
                url,
                {
                    mode: "cors",
                    method: "POST",
                    credentials: "include",
                    signal: upload.controller.signal,
                    body: editor.value,
                    headers: {
                        "content-type": "text/plain charset=utf-8"
                    }
                }
            );
            
        if (!await checkResponse(response))
            return;
            
        var json = await response.json();
        
        alert(json);
    }
    catch(error) {
        alert(error);
    }
    finally {
        upload.controller = undefined;
    }
}

// Check fetch response errors
// and logon redirects
async function checkResponse(response) {

    
    if (response.status == 200)
        return true;
        
    // Unauthorised 
    if (response.status == 401) 
    {
        var json = await response.json();
        redirect(json);
        return false;
    }
    // Not ok
    else if (response.status != 200) {
        var json = await response.json();
        throw new Error(
            decodeURIComponent(json)
        );
        return false;
    }

}

document.
    getElementById("a")
    .innerText =
        document
        .location
        .origin;
        
</script>

             
    </body>

</html>

