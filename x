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
                    <a href="#?deaths and heroin and male">deaths and heroin and male</a>
                </td>
            </tr>
            <tr>
                <td>
                    <a href="#?deaths and heroin and female and not hospital">deaths and heroin and female and not hospital</a>
                </td>
            </tr>
            <tr>
                <td>
                    <a href="#?deaths and (alcohol or heroin)">deaths and (heroin or alcohol)</a>
                </td>
            </tr>
        </table>
        
        <form id="download">
            <input id="input" value="my"></input>
            <button type="submit">fetch</button>
        </form>
        
        <form id="upload" style="display:none" >
            <textarea id="editor"></textarea>
            <button type="submit">save</button>
        </form>
        
        <form id="results">
            <table id="table">
            </table>
        </form>


        
        <script type="module">
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
    




download.onsubmit =
async (event) => {
    try {
    
         
        event.preventDefault();
        
        table.innerHTML = "";
        editor.value = "";
        
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

            var json = await
                response.json();
                
            if (typeof json == "number")
            {
            
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
                
                var number = json;
                
                tds[1].innerText = 
                    String(number);
                
                    
                return;
            }
            
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
                    
                    var params = new URLSearchParams(
                        url.search
                    );
                    
                    if (params.has("next"))
                        text = "...";
                    
                    a.href = "#" + getShortURL(url);
                    a.innerText = text;
                }
            );
        }
        else
        {
            upload.style.display = "block";
            editor.style.display = "block";
            results.style.display = "none";
            
            var text = await response.text();
        
            if (text != undefined)
                editor.value = text;
            else
                editor.value = "undefined";
        }
        
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
    try {
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
                        "content-type": 
                        "text/html; charset=utf-8"
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

input.oninput =
() => {

    var url = getShortURL();
    a.href = url;
    
    var text = url;
    if (text == "/") {
        var url =
            new URL(
                input.value,
                document.location
            );
        text = url.hostname;
    }
    
    a.innerText = text;

    
}

input.onchange =
() => {
    window.location.hash =
        "#" + getShortURL()
}
        
window.onhashchange =
async () => {

    var hash =
        document
        .location
        .hash;
        
    if (hash.length)
        hash = hash.substr(1);
    else
        hash = "my";
        
    hash = decodeURIComponent(hash);
    
    if (input.value != hash)
    {
        input.value = hash;
    
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
    
    if (url.pathname == "/x")
        url.pathname = "/";
    
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

window.onhashchange();
input.oninput();

</script>

             
    </body>

</html>

