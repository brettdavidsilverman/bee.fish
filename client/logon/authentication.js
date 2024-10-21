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
      
      var parameters = {
         method: "POST",
         credentials: "include",
         body: JSON.stringify({
            method: "logon",
            secret: secret
         })
      }

      var promise =  
         fetch(this.url, parameters)
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
         );

      return promise;
   }
   
   getStatus()
   {
      var _this = this;
      
      this.authenticated = false;
      
      var parameters = {
         method: "POST",
         credentials: "include",
         body: JSON.stringify({
            method: "getStatus",
         })
      }
         
      var _this = this;

      var promise =
         fetch(this.url, parameters)
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
         );

      return promise;
   }
   
   logoff()
   {
      var _this = this;
      
      this.authenticated = false;

      var parameters = {
         method: "POST",
         credentials: "include",
         body: JSON.stringify({
            method: "logoff"
         })
      }
         
      var promise =
         fetch(this.url, parameters)
         .then(
            function(response) {
               response.json();
            }
         );
         
      this.authenticated = false;
      this.secret = null;
      document.cookie = "sessionId=;path=/;max-age=0;"
      return promise;
   }
   
}

function authenticate() {

   var authentication =
      new Authentication(
         document.location.origin
      );
   var promise =
      authentication.getStatus().
      then(
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
            Error(error, authenticate);
          }
      );
      
   return promise;

}

