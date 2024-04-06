class IfConnectorTool extends ConnectorTool {

   form;

   constructor(input) {
      super(input);
   }

   async draw(context) {

      await super.draw(context);

      context.fillStyle = "black";
      context.strokeStyle = "black";

      this.drawText(context, "", 40, "Courier New", "⬊", true, true);

      context.fillStyle = "green";
      context.strokeStyle = "green";

      this.drawText(context, "Bold", 50, "Courier New", "?", true, true);

   }

   createConnector(from, to) {
      return new IfConnector(
         {
            connector: {
               from,
               to
            }
         }
      );

   }

   async click(point) {
      if (  this.selection instanceof Connector &&
            !(this.selection instanceof IfConnector ) ) {

         if (confirm("Convert connector to if connector?")) {
            var connector = this.selection;
            var newConnector = new IfConnector(
               {
                  connector
               }
            );
            var parent = connector.parent;
            parent.children.remove(connector);
            parent.children.push(newConnector);
            newConnector.save();
            parent.save();
            this.onjoin(newConnector);
         }

         return;
      }
      else {
         super.click(point);
      }
   }

   async onjoin(connector) {
      var form = await connector.form.fetch();
      form.inputs.push(connector.from);
      form.selected = true;
      this.toolbox.parent.selection.selected = false;
      this.toolbox.parent.selection = form;
      this.toolbox.parent.draw();

   }

}
