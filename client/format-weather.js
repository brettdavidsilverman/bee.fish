function formatWeather(json, table) {

    for (var item in json) {

        if (item == "extended") {
            formatWeather(json.extended, table);
            continue;
        }

        var object = json[item];

        var tr = document.createElement("tr");

        if ( ( item == "Weather 1" ) ||
             ( item == "Weather 2") )
        {
            var td = document.createElement("td");
            td.colSpan = 3;
            td.align = "center";
            td.innerText = item;
            tr.appendChild(td);
            table.appendChild(tr);
            formatWeather(object, table);
            tr = document.createElement("td");
            table.appendChild(tr);
            continue;
        }


        if (object.unit == "url") {
            var td = document.createElement("td");
            td.colSpan = "3";
            var a = document.createElement("a");
            a.href = object.value;
            var text = document.createTextNode(object.label);
            a.appendChild(text);
            td.appendChild(a);
            td.align = "left";
            tr.appendChild(td);
        }
        else {
            var tdLabel = document.createElement("td");
            tdLabel.appendChild(document.createTextNode(item));
            tr.appendChild(tdLabel);
            
            var tdValue = document.createElement("td");
            var value = object.value;
            switch (typeof(value)) {
            case "string":
                tdValue.align = "left";
                if (value == "NaN")
                    tdValue.aign = "right";
                break;
            case "number":
                tdValue.align = "right";
                if (object.precision != undefined) {
                    value = value.toFixed(object.precision);
                }
                break;
            default:
                tdValue.align = "center";
            }
            tdValue.appendChild(document.createTextNode(value));
            tr.appendChild(tdValue);
    
            if (object.unit) {
                var tdUnit = document.createElement("td");
                tdUnit.appendChild(document.createTextNode(object.unit));
                tdUnit.align = "left";
                tr.appendChild(tdUnit);
            }
            else
                tdValue.colSpan = "2";
    
        }
        
        table.appendChild(tr);
    }
}
