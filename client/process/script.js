var connectFrom;
var connectButton;
function createProcess(label) {

   

   var process =
      processes[label];

   var json = {
      "form": {
         "id": makeId(label),
         "className": "movable",
         "style": {
            "position": "absolute",
            "top": process.topLeft.y + "px",
            "left": process.topLeft.x + "px"
         },
         "onmoving":
function (event) {
   process.topLeft = event.point;
   window.onresize();
},
         "onsubmit":
function() {
   return false;
},
         "children": [
            {
               "div": {
                  "id": makeId(label, "inputsDiv"),
                  "className": "inputs",
                  "children": createInputLabelsJSON(label)
               },
               "label": {
                  "id": makeId(label, "label"),
                  "contentEditable": "true",
                  "innerText": label,
                  "onblur":
function (event) {
   var _label =
      document.getElementById(
         makeId(label, "label")
      );
      
   var newLabel =
      _label.innerText;
   
   if (newLabel == label)
      return;
      
   var test = document.getElementById(newLabel);
   if (test) {
      alert(newLabel + " already exists");
      _label.innerText = label;
      return;
   }
   
   changeLabel(label, newLabel);
}
               }
            },
            {
               "br": {}
            },
            {
               "select": {
                  "style": {
                     "float": "left"
                  },
                  "children": [
                     createOption("Function", "function", process.type == "function"),
                     createOption("Branch", "branch", process.type == "branch"),
                     createOption("Input", "input", process.type == "input"),
                     createOption("Output", "output", process.type == "output"),
                     createOption("Variable", "variable", process.type == "variable")
                  ],
                  "onchange":
function(event) {
   onProcessTypeChange(this);
}
    
               }
            },
            createFunctionDiv(),
            createBranchDiv(),
            createInputDiv(),
            createOutputDiv(),
            createVariableDiv(),
            
            {
               "div": {
                  "className": "buttons",
                  "children": [
                     {
                        "button": {
                           "id": makeId(label, "connectButton"),
                           "className": "connect",
                           "innerText": "connect",
                           "onclick":
function(event) {
   var buttonId = makeId(label, "connectButton");
   var button = document.getElementById(buttonId);
   
   
   var fromId = makeId(label);
   connectFrom =
      document.getElementById(fromId);
   if (connectButton)
      connectButton.classList.remove("connecting");
      
   connectButton = button;
   connectButton.classList.add("connecting");
    
   alert("Click connect on a processes input");
   

}
                     }
                  },
                  {
                     "button": {
                        "className": "delete",
                        "innerText": "Delete",
                        "onclick":
function (event) {
   if (!confirm("Delete " + label + "?"))
      return false;
      
   var form = document.getElementById(makeId(label));
   form.remove();
   delete processes[label];
   
   if (!processes.routes)
      processes.routes = [];
      
   var routes = processes.routes;
   
   for (var index = 0;
        index < routes.length;
        )
   {
      var route = routes[index];
      if (route.from == label ||
          route.to == label)
      {
         routes.splice(index, 1);
      }
      else
         ++index;
      
   }
   
   createRoutes();
   
   return true;
   
}
                     }
                  },
                  {
                     "button": {
                        "className": "go",
                        "innerText": "Go",
                        "onclick":
function (event) {
   var process = processes[label];
   var form = document.getElementById(makeId(label));
      setFormClass(label);
   
   if (process.function) {
      if (!process.f) {
         compileFunction(label);
      }
      if (process.f) {
         process.f();
         setFormClass(label, "complete");
      }
   }
   else if (process.input) {
      var inputId = makeId(label, "input");
      var input = document.getElementById(inputId);
      process.value = input.value;
      setFormClass(label, "complete");
   }
}
                        }
                     }
                  ]
               }
            }
         ]
      }
      
   };
   
   var div = document.getElementById("processesDiv");
   HTML(json, div);
   
      // Create the function div json
   function createFunctionDiv()
   {

      var functionDiv =
      {
         "div": {
            "id": makeId(label, "functionDiv"),
            "style": {
               "display": process.type == "function" ? "block" : "none"
            },
            "children": [
               {
                  "textarea": {
                     "id": makeId(label, "function"),
                     "innerHTML": process.function,
                     "oninput":
function(event) {
   setFormClass(label, "ready");
},
                     "onchange":
function(event) {
   process.function = this.value;
   delete process.f;
   return true;
}
                  }
               }
            ]
         }
      };
      
      return functionDiv;
     
   }
   
   // Create the branch div json
   function createBranchDiv()
   {
       
      var branchDiv =
      {
         "div": {
            "id": makeId(label, "branchDiv"),
            "className": "branch",
            "style": {
               "display": process.type == "branch" ? "block" : "none",
            },
            "children": [
       
               {
                  "svg": {
                     "id": makeId(label, "svg"),
                     "attributes": {
                        "width": "300",
                        "height": "150"
                     },
                     "className": "branchSVG",
                     "children": [
                        {
                           "g": {
                              "attributes": {
                                 "transform": "rotate(45, 50, 50) translate(70, 00) scale(0.5, 0.5)"
                              },
                              "children" : [
                                 {
                                    "rect": {
                                       "attributes" : {
                                          "width": "100",
                                          "height": "100",
                                          "x": "0",
                                          "y": "0",
                                          "rx": "0",
                                          "ry": "0",
                                          "fill": "red",
                                          "stroke": "black"
                                       }
                                    }
                                 }
                              ]
                           }
                        }
                        
                     ]
                  }
               },
               {
                  "textarea": {
                     "id": makeId(label, "function"),
                     "className": "branchTextArea",
                     "innerHTML": process.function,
                     "oninput":
function(event) {
   setFormClass(label, "ready");
},
                     "onchange":
function(event) {
   process.function = this.value;
   delete process.f;
   return true;
}
                  }
               },
               {
                  "select": {
                     "id": makeId(label, "branchGotoSelect"),
                     "className": "branchGotoSelect",
                     "children": [
                        createBranchOptions()
                     ],
                     "onfocus":
function(event) {
   var select = this;
   select.innerHTML = "";
   for (var _label in processes) {
      if (_label != label &&
          _label != "routes" &&
          _label != "save")
         select.add(
            new Option(
               _label,
               makeId(_label),
               false,
               _label == processes[label]["goto"]
            )
         );
   }
},
                     "onchange":
function(event) {
   var process = processes[label];
   proces["goto"] = this.value;
}
                  }
               }
            ]
         }
      };
      
       
      return branchDiv;
      
      function createBranchOptions() {
         var options = [];
         options.push(
            createOption("Select goto", undefined)
         );
         
         for (var label in processes) {
            if (label != "routes" && label != "save") {
               options.push(
                  createOption(label, label, process["goto"] == label)
               );
            }
         }
         
         return options;
      }
   }
   
   
   // Create the input div json
   function createInputDiv() {
      var inputDiv = {
         "div": {
            "id": makeId(label, "inputDiv"),
            "style": {
               "display": process.type == "input" ? "block" : "none"
            },
            "children": [
               {
                  "select": {
                     "onchange":
function (event) {
   var inputId = makeId(label, "input");
   var input = document.getElementById(inputId);
   
   input.type = this.value;
   input.value = "";
   process.input = input.type;
   delete process.value;

   setFormClass(label, "ready");
},
                     "children": [
                        createOption("Button", "button", process["input"] == "button"),
                        createOption("Check box", "checkbox", process["input"] == "checkbox"),
                        createOption("Color", "color", process["input"] == "color"),
                        createOption("Date", "date", process["input"] == "date"),
                        createOption("Date time", "datetime-local", process["input"] == "datetime-local"),
                        createOption("Email", "email", process["input"] == "email"),
                        createOption("File", "file", process["input"] == "file"),
                        createOption("Image", "image", process["input"] == "image"),
                        createOption("Month", "month", process["input"] == "month"),
                        createOption("Number", "number", process["input"] == "number"),
                        createOption("Password", "password", process["input"] == "password"),
                        createOption("Radio", "radio", process["input"] == "radio"),
                        createOption("Range", "range", process["input"] == "range"),
                        createOption("Reset", "reset", process["input"] == "reset"),
                        createOption("Search", "search", process["input"] == "search"),
                        createOption("Submit", "submit", process["input"] == "submit"),
                        createOption("Telephone", "tel", process["input"] == "tel"),
                        createOption("Text", "text", process["input"] == "text"),
                        createOption("Time", "time", process["input"] == "button"),
                        createOption("URL", "url", process["input"] == "time"),
                        createOption("Week", "week", process["input"] == "week"),
                     ]
                  }
               },
               {
                  "input": {
                     "id": makeId(label, "input"),
                     "type": process.input,
                     "value": process.value == undefined ? "" : process.value
                  }
               }
            ]
         }
      };
      
      return inputDiv;
   }
   
      // Create the function div json
   function createOutputDiv()
   {

      var outputDiv =
      {
         "div": {
            "id": makeId(label, "outputDiv"),
            "style": {
               "display": process.type == "output" ? "block" : "none"
            },
            "children": [
               {
                  "textarea": {
                     "id": makeId(label, "output"),
                     "innerHTML": process.function,
                     "oninput":
function(event) {
   setFormClass(label, "ready");
},
                     "onchange":
function(event) {
   process.function = this.value;
   delete process.f;
   return true;
}
                  }
               }
            ]
         }
      };
      
      return outputDiv;
     
   }
   
   // Create the variable div json
   function createVariableDiv()
   {

      var variableDiv =
      {
         "div": {
            "id": makeId(label, "variableDiv"),
            "style": {
               "display": process.type == "variable" ? "block" : "none"
            },
            "children": [
               {
                  "textarea": {
                     "id": makeId(label, "function"),
                     "innerHTML": JSON.stringify(process.initial, null, "   "),
                     "oninput":
function(event) {
   setFormClass(label, "ready");
},
                     "onchange":
function(event) {
   process.initial = JSON.parse(this.value);
   return true;
}
                  }
               }
            ]
         }
      };
      
      return variableDiv;
     
   }
   
   function createInputLabelsJSON(label)
   {
      var inputLabels = [];
      var process = processes[label];
      
      if (!process.inputs)
         process.inputs = [];
         
      var inputs = process.inputs;
      for (var i = 0; i < inputs.length; ++i)
      {
         var input = inputs[i];
         inputLabels.push(
            {
               "span": {
                  "id": makeId(label, input),
                  "innerText": input,
                  "onclick":
function (event) {
   var span = event.target;
   var inputLabel = span.innerText;
   var refresh = false;
   
   if (connectFrom) {
      
      var connectTo = document.getElementById(makeId(label));
      if (connectFrom != connectTo) {
          
         if (!processes.routes)
            processes.routes = [];
            
         var routes = processes.routes;
         
         // Check if this route already exists
         var exists = false;
         for (var index = 0;
              index < routes.length;
              ++index)
         {
            
            var route = routes[index];
            if (route.from == connectFrom.id &&
                route.to == connectTo.id)
            {
               if (confirm("This route exists. Delete it?"))
               {
                  routes.splice(index, 1);
               }
               exists = true;
            }
         }
         
         if (!exists) {
            if (confirm("Connect " + connectFrom.id + " to " + label + "." + inputLabel + "?"))
            {
               routes.push(
                  {
                     from: connectFrom.id,
                     to: connectTo.id,
                     label: inputLabel
                  }
               );
            }
         }
         
         refresh = true;
      }
      
      
   }
   else if (confirm("Delete input " +
               inputLabel))
   {
      var process = processes[label];
      var i;
      
      if (!process.inputs)
         process.inputs = [];
         
      // Delete this processes input
      for (i = 0;
           i < process.inputs.length;
           ++i)
      {
         if (process.inputs[i] == inputLabel)
         {
            process.inputs.splice(i, 1);
            break;
         }
      }
      
      // Remove any routes that use this input
      if (!processes.routes)
         processes.routes = [];
         
      // Delete this processes input
      for (i = 0;
           i < processes.routes.length;
           ++i)
      {
         var route = processes.routes[i];
         
         if (route.to == label &&
             route.label == inputLabel)
         {
            processes.routes.splice(i, 1);
         }
      }
      
      refresh = true;
   }
   
   if (refresh) {
      createInputLabels(label);
      createRoutes();
   }
   
   if (connectButton)
      connectButton.classList.remove("connecting");
         
   connectFrom = null;
   connectButton = null;
   
   return false;
   
}
               }
            }
         );
  
      }
      inputLabels.push(
         {
            "button": {
               "id": makeId(label, "addInputButton"),
               "innerText": "+",
               "onclick":
function(event) {
   var inputLabel = prompt("Input label?");
   
   if (!inputLabel)
      return false;
      
   var inputLabelId = makeId(label, inputLabel);
   
   while (document.getElementById(inputLabelId))
   {
      inputLabel = prompt("Input label exists?", inputLabel);
      
      if (!inputLabel)
         return false;
         
      inputLabelId =
         makeId(label, inputLabel);
   }
      
   var process = processes[label];
   if (!process.inputs)
      process.inputs = [];
      
   process.inputs.push(inputLabel);
   
   createInputLabels(label);
}
            }
         }
      );
      return inputLabels;
   }
   
   function createInputLabels(label) {
      // Update the input labels
      var inputsDivId = makeId(label, "inputsDiv");

      var inputsDiv = document.getElementById(
         inputsDivId
      );
         
      inputsDiv.innerHTML = "";
         
      var inputLabels = createInputLabelsJSON(label);
         
      for (var i = 0;
           i < inputLabels.length;
           ++i)
      { 
         HTML(inputLabels[i], inputsDiv);
      }
   }
   
   function onProcessTypeChange(select) {

      var allDivs = [
         makeId(label, "functionDiv"),
         makeId(label, "branchDiv"),
         makeId(label, "inputDiv"),
         makeId(label, "outputDiv"),
         makeId(label, "variableDiv")
      ];
      
      // Hide all
      for (var i = 0; i < allDivs.length; ++i) {
         var id = allDivs[i];
         var div = document.getElementById(id);
         div.style.display = "none";
      }
   
      // Display selected
      var selected = select.value;
      var id = makeId(label, selected + "Div");
      var div = document.getElementById(id);
   
      div.style.display = "block";
      
      createRoutes();
   }
   
}

function makeId(label, id) {
    
   var _id = label + (id ? " " + id : "");

   //_id = _id.replaceAll(" ", "_");
  
   return _id;}

function setFormClass(label, className) {
   var form = document.getElementById(makeId(label));
   form.classList.remove("error");
   form.classList.remove("ready");
   form.classList.remove("complete");
   if (className)
      form.classList.add(className);
}


function createOption(label, value, selected=undefined) {
   return {
      "option": {
         "value": value,
         "innerText": label,
         "selected": selected ? "true" : undefined
      }
   }
}

function compileFunction(label)
{
   var process = processes[label];
   
   delete process.f;
   
   try {
      var func = new Function("return " + process.function);
      var f = func();
      if (typeof f == "function") {
         process.f = f;
         setFormClass(label, "ready");
         return true;
      }
      throw "Text must be a function";
   }
   catch (error)
   {
      Error(error, compileFunction);
      setFormClass(label, "error");
      return false;
   }
   
   
}
   

function onNewProcess(event) {
   var label = "Process ";
   var count = 1;
   while (processes[label + count])
      ++count;
      
   label += count;
   
   while (true) {
      label = prompt("New process", label);
      if (!label)
         return;
         
      if (processes[label])
         alert(label + " already exists");
      else
         break;
         
   };
   
   var point = coordinates.getCoordinates(event);
   
   processes[label] = {
      "type": "function",
      "inputs": [],
      "function": "function() {\r\n   alert('hello');\r\n}",
      "topLeft": {
         "x": point.x - 125,
         "y": point.y
      }
   };
   
   createProcess(label);
   
   return true;
   
}
function createProcesses() {
   
   var div = document.getElementById("processesDiv");
   
   div.innerHTML = "";
   
   for (var label in processes) {
      if (label != "routes" &&
          label != "save")
      {
         createProcess(label);
      }
   }
   window.onresize();
}

function createRoutes() {
   var svg = document.getElementById("processesSVG");
   svg.innerHTML = "";
   
   if (!processes.routes)
      processes.routes = [];
      
   for (var index = 0;
        index < processes.routes.length;
        ++index)
   {
      var route = processes.routes[index];
      createRoute(route);
   }
   
   
   
}

function createRoute(route) {
    
   var svg = document.getElementById("processesSVG");
   
   var from = document.getElementById(makeId(route.from));
   var to = document.getElementById(makeId(route.to, route.label));
   var fromRect = from.getBoundingClientRect();
   var toRect = to.getBoundingClientRect();
   
   var line = document.createElementNS("http://www.w3.org/2000/svg", "line");
   var _from = processes[route.from].topLeft;
   var _to = processes[route.to].topLeft;

   line.setAttribute("x1", _from.x + fromRect.width / 2);
   line.setAttribute("y1", _from.y + fromRect.height);
   line.setAttribute("x2", toRect.x + toRect.width / 2);
   line.setAttribute("y2", toRect.y);
   line.setAttribute("stroke", "blue");
   
   svg.appendChild(line);
   
}

function changeLabel(oldLabel, newLabel) {
   var oldForm = document.getElementById(makeId(oldLabel));
   oldForm.remove();
   var oldProcess = processes[oldLabel];
   delete processes[oldLabel];
   processes[newLabel] = oldProcess;
   
   
   // reroute routes
   if (!processes.routes)
      processes.routes = [];
      
   var routes = processes.routes;
   
   for (var index = 0;
        index < routes.length;
        ++index)
   {
      var route = routes[index];
      if (route.from == oldLabel)
         route.from = newLabel;
      if (route.to == oldLabel)
         route.to = newLabel;
   }
   
   // reroute gotos
   for (var label in processes)
   {
      var process = processes[label];
      if (process.type == "branch")
      {
         if (proces["goto"] == oldLabel)
         {
            proces["goto"] = newLabel;
         }
         document.getElementById(makeId(label)).
            remove();
         createProcess(label);
         
      }
   }
   
   createProcess(newLabel);
   createRoutes();
}
function setupProcessEditor()
{
   var h1 = document.getElementById("h1");
   var svg = document.getElementById("processesSVG");
   var div = document.getElementById("processesDiv");
   
   
   if (!svg) {
      svg = document.createElementNS(
         "http://www.w3.org/2000/svg",
         "svg"
      );
      svg.id = "processesSVG";
      
      document.body.insertBefore(svg, h1);
      
   }
   if (!div) {
      div = document.createElement("div");
      div.id = "processesDiv";
      document.body.insertBefore(div, h1);
   }
   
   
   svg.onclick =
   function (event) {
      if (connectFrom)
      {
         connectFrom = null;
         if (connectButton)
            connectButton.classList.
               remove("connecting");
         connectButton = null;
         return false;
      }
      
      return onNewProcess(event);
   }}

window.onresize =
   function(event) {
      var maxWidth = 0;
      var maxHeight = 0;
      for (var label in processes) {
         if (label != "routes" && label != "save") {
            var form = document.getElementById(makeId(label));
            var rect = form.getBoundingClientRect();
            var topLeft = processes[label].topLeft;
            if (topLeft.x + rect.width > maxWidth)
               maxWidth = topLeft.x + rect.width;
               
            if (topLeft.y + rect.height > maxHeight)
               maxHeight = topLeft.y + rect.height;
         }
      }

      document.body.style.width = maxWidth * 2 + "px";
      document.body.style.height = maxHeight * 2 + "px";

      var svg = document.getElementById("processesSVG");
      
      svg.style.width = document.body.style.width;
      svg.style.height = document.body.style.height;
    
      var div = document.getElementById("processesDiv");
      div.style.width = document.body.style.width;
      div.style.height = document.body.style.height;
      
      createRoutes();
   }
   
