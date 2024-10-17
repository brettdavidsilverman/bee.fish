class Authentication
{
   constructor(authenticatorServer = document.location.origin) {
     // Object.assign(this, input);
      this.authenticated = false;
      this.url = authenticatorServer;
   }
   
   logon(secret)
   {
   
      var _this = this;
      
      this.authenticated = false;
      
      if ( secret == null || !secret.length )
         throw new Error("Missing secret");
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      /*
      params.headers = new Headers([
         ["Content-Type", "application/json; charset=utf-8"]
      ]);
      */
      var body =
         {
            method: "logon",
            secret: secret
         }
         
      params.body = JSON.stringify(body);

      var promise =  
         fetch(this.url, params)
         .then(
            function(response) {
               return response.json();
            }
         )
         .then(
            function(json) {
               _this.authenticated = 
                  json.authenticated;
               _this.sessionId =
                  json.sessionId;
            }
         )
         .catch(
            function(error) {
               throw new Error(error);
            }
         );

      return promise;
   }
   
   getStatus()
   {
      var _this = this;
      
      this.authenticated = false;
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "getStatus"
         }
         
      params.body = JSON.stringify(body);
      var _this = this;

      var promise =
         fetch(this.url, params)
         .then(
            function(response) {
               return response.text()
            }
         )
         .then(
            function(text)
            {
               return JSON.parse(text);
            }
         )
         .then(
            function(json) {
               _this.authenticated =
                  json.authenticated;
               _this.sessionId =
                  json.sessionId;
               
               return json;
            }
         )
         .catch(
            function(error) {
               throw new Error(error);
            }
         );

      return promise;
   }
   
   logoff()
   {
      var _this = this;
      
      this.authenticated = false;

      var params = {}
      params.method = "POST";
      params.credentials = "include";
      var body =
         {
            method: "logoff"
         }
         
      params.body = JSON.stringify(body);

      var promise =
         fetch(this.url, params)
         .then(
            function(response) {
               response.json();
            }
         )
         .catch(
            function(error) {
               throw new Error(error);
            }
         );

         
      this.authenticated = false;
      this.secret = null;
      document.cookie = "sessionId=;path=/;max-age=0;"
      return promise;
   }
   
}

var authentication = new Authentication("https://bee.fish:8000");

async function authenticate() {

    return await authentication.getStatus()
    .then(
        function(auth) {
            if (!auth.authenticated) {
                
               var currentPage = document.location.href;
               var newPage = authentication.url + "/client/logon/";
               var url = newPage + "?redirect=" + encodeURIComponent(currentPage);
               document.location.href = url;
            }
            return auth;
        }
    )
    .catch (
       function(error) {
          Error(error);
       }
    );

}


