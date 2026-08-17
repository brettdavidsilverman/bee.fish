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

#console {
    text-align: right;
    z-index: 1;
}

        </style>

    </head>
    <body>
    
        <h1 id="h1">
            <a id="a">
                bee.fish
            </a>
        </h1>

        <table id="samples">
            <tr>
                <th>
                    Sample queries
                </th>
            </tr>
            <tr>
                <td>
                    <a href="#?deaths and heroin and male">?deaths and heroin and male</a>
                </td>
            </tr>
            <tr>
                <td>
                    <a href="#?deaths and heroin and female and not hospital">?deaths and heroin and female and not hospital</a>
                </td>
            </tr>
            <tr>
                <td>
                    <a href="#?deaths and (alcohol or heroin)">?deaths and (heroin or alcohol)</a>
                </td>
            </tr>
        </table>
        
        <form id="download">
            <input id="input" value="my"></input>
            <button type="submit" id="downloadButton">fetch</button>
        </form>
        
        <form id="upload" style="display:none" >
            <textarea id="editor"></textarea>
            <button type="submit" id="uploadButton">save</button>
        </form>
        
        <form id="results">
            <table id="table">
            </table>
        </form>


        
        <script type="module">
var console = new Console();
console.log("Hello World");

const a =
    document
    .getElementById("a");
    
const download =
    document
    .getElementById("download");
    
const input =
    document
    .getElementById("input");
    
const results =
    document
    .getElementById("results");
    
const table =
    document
    .getElementById("table");
    
const upload =
    document
    .getElementById("upload");
    
const editor =
    document
    .getElementById("editor");
    
const downloadButton =
    document
    .getElementById("downloadButton");

const uploadButton =
    document
    .getElementById("uploadButton");

upload.onsubmit =
async (event) => {
    
    try {
    
        uploadButton.disabled = true;
        
        
        event.preventDefault();
        
        if (upload.controller)
            upload.controller.abort("User cancelled");
            
        upload.controller =
            new AbortController();
            
        const {signal} = upload.controller;
        
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
                        "content-type": 
                        "text/html; charset=utf-8"
                    }
                }
            );
            
        upload.controller = null;
        
        if (!await checkResponse(response))
            return;
            
        var json = await response.json();
        
        alert(json);
    }
    catch(error) {
        alert(error);
    }
    finally {
        uploadButton
            .disabled 
            = false;
    }
}

download.onsubmit =
async (event) => {

    try {
        event.preventDefault();
        
        downloadButton.disabled = true;
        
        window.setTimeout(
            () => {
                downloadButton
                    .disabled 
                    = false;
            },
            200
        );
            
        table.innerHTML = "";
        editor.value = "";
        
        input.oninput();

        var url = new URL(
            input.value,
            document.location.origin
        );
        
        if (download.controller) {
            download.controller.abort("User cancelled");
        }
        
        download.controller = 
           new AbortController();
           
        const {signal} =
            download.controller;
        
        var response = await
            fetch(
                url,
                {
                    mode: "cors",
                    method: "GET",
                    credentials: "include",
                    signal: signal
                }
            );
            
        download.controller = null;
            
        upload.style.display = "block";
        results.style.display = "none";
            
            
        // This checks for login
        // or error
        if (!await checkResponse(response))
            return;
            
        // Handle queries as array
        // of urls
        if (url.search.length)
        {
            upload.style.display = "none";
            results.style.display = "block";
    
            if (url.search.endsWith("$"))
                await downloadCountResults(response);
            else
                await downloadSearchResults(response);
        }
        else
        {
            await downloadData(response);
        }
        
    }
    catch(error)
    {
        alert(error);
    }
    finally {
        
        download.controller = null;

        downloadButton
            .disabled 
            = false;
            
    }
}

const downloadSearchResults =
async (response) => {
    
    var json = await
        response.json();

    var array = json;
                
    if (array.length == 0)
    {
        alert("No results");
        return;
    }
                
    array.forEach(
        (item, key) => {
                
            var url = 
                new URL(item);

            addSearchItem(url);
        }
    );
}

const addSearchItem =
(url) => {
    var a =
        document.createElement("a");
    var row =
        document.createElement("tr");
    var td =
        document.createElement("td");
                        
    table.append(row);
    row.append(td);
    td.append(a);
                    
    var text = getShortURL(url);
    if (url.searchParams.has("next"))
        text = "Next...";
                    
    a.href = "#" + getShortURL(url);
    a.innerText = text;
}

const downloadCountResults =
async (response) => {
    var number = await response.json();

    var row =
        document
        .createElement("tr");
                    
    var tds = [
        document
        .createElement("td"),
        document
        .createElement("td")
    ];
                
    var a =
        document
        .createElement("a");
                        
                    
    table.append(row);
    row.append(tds[0]);
    row.append(tds[1]);
                
    a.href = "#" + input.value;
    a.innerText = input.value;
                    
    tds[0].append(a);
                
    tds[1].innerText = 
        String(number);
                
}

const downloadData =
async (response) => {
    upload.style.display = "block";
    editor.style.display = "block";
    results.style.display = "none";
            
    var text = await response.text();
        
    if (text != undefined)
        editor.value = text;
    else
        editor.value = "undefined";
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
            json
        );
        return false;
    }

}

function redirect(path)
{
    var url = new URL(path);
    url.search = "?redirect=" +
        encodeURIComponent(document.location.href);
    document.location.replace(url);
}

input.onchange =
async () => {
    window.location.hash =
        "#" + getShortURL();
}
    
input.oninput =
() => {
    a.href = input.value;
    a.innerText = getTextByURL(
        input.value
    );
}

window.onhashchange =
async () => {

    var hash =
        document
        .location
        .hash;
        
        
    // Remove #
    if (hash.length)
        hash = hash.substr(1);

    // Remove preceeding /
    if (hash.startsWith("/") &&
        hash != "/")
    {
        hash = hash.substr(1);
    }
    
    if (hash == "")
        hash = "my";
    
    hash = decodeURIComponent(hash);
    
    var oldURL = new URL(
        input.value,
        document.location
    );
    
    var newURL = new URL(
        hash,
        document.location
    );
    
    if (oldURL.toString() != 
        newURL.toString()
    )
    {
        input.value = hash;

        a.href = hash;
        a.innerText = getTextByURL(hash);
        
        download.requestSubmit();
    }
    
}

var getShortURL =
(url) => {

    if (url == undefined) {
        url = new URL(
            input.value,
            document.location
        );
    }

    if (url.origin == 
            document
            .location
            .origin)
    {
        url =
            url.pathname +
            url.search;
    }
    else
        url = url.toString();
        
    url = decodeURIComponent(url);
    
    return url;
}

const getTextByURL =
(url) => {
    
    if (url == "/" || url == "") {
        var url =
            new URL(
                input.value,
                document.location
            );
        return url.hostname;
    }
    else if (url.startsWith("/"))
        return url.substr(1);
    else if (url.length)
        return url;
}

download.requestSubmit();



</script>

             
    </body>

</html>

