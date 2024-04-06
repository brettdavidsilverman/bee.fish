class Code {
   #pre;
   #ok;
   #cancel;
   #statement;
   #changed = false;
   constructor(input) {
      Object.assign(this, input);
      
      var code = this;
      
      createOk();
      createCancel();
      createPre();
      
      function createPre() {
         code.#pre =
            document.createElement("pre");
      
         var textNode =
            document.createTextNode(
               code.#statement
            );
            
         code.#pre.appendChild(
            textNode
         );
      
         code.#pre.style.position =
            "absolute";
            
         code.#pre.style.opacity = "0.75";
      
         code.#pre.style.backgroundColor =
            "white";
         code.#pre.style.fontSize = "5mm";
         code.#pre.style.margin = "2mm";
         code.#pre.style.border =
            "thick solid #000000";
         
         code.#pre.style.top = "0mm";
         code.#pre.style.left = "0mm";
         code.#pre.style.right = "0mm";
         code.#pre.style.bottom = "0mm";
         code.#pre.style.overflow = "scroll";
  
         code.#pre.style.zIndex = "2";
         code.#pre.contentEditable = true;
         
                
         document.body.appendChild(
            code.#pre
         );
      
         code.#pre.oninput = function() {
            code.#changed = true;
         }
         
         code.#pre.focus();
      }
      
      function createOk() {
         code.#ok = createButton();
         code.#ok.style.backgroundColor =
            "green";
         code.#ok.style.right = "0mm";
         code.#ok.onclick = function() {
         
            if (!code.#changed) {
               code.remove();
               return;
            }
            
            if (confirm("Save changes?")) {
               code.save(code.statement);
               code.remove();
            }
         }
         document.body.appendChild(
            code.#ok
         );
      }
      
      function createCancel() {
         code.#cancel = createButton();
         code.#cancel.style.backgroundColor =
            "red";
         code.#cancel.style.left = "0mm";
         code.#cancel.onclick = function() {
           
            if (!code.#changed) {
               code.remove();
               return;
            }
            
            if (confirm("Cancel?"))
               code.remove();
               
         }
         
         document.body.appendChild(
            code.#cancel
         );
      }
      
      function createButton() {
         var button =
            document.createElement("div");
         button.style.position =
            "absolute";
         button.style.zIndex = "3";
         button.style.backgroundColor =
            "grey";
         button.style.height = "10mm";
         button.style.width = "10mm";
         button.style.bottom = "0mm";
         button.style.margin = "4mm";
         button.style.border =
            "thick solid #000000";
         
         return button;
      }
   }
   
   remove() {
      this.#ok.remove();
      this.#cancel.remove();
      this.#pre.remove();
   }
   
   save(statement) {
   }
   
   get statement() {
      return this.#pre.innerText;
   }
   
   set statement(value) {
      this.#statement = value;
   }
   

}