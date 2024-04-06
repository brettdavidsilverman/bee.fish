class Authentication
{
   constructor(input) {
      Object.assign(this, input);
      this._authenticated = false;
   
      this.url = document.location.origin;
      
   }
   
   logon(secret)
   {
   
      var _this = this;
      
      this._authenticated = false;
      
      if ( secret == null || !secret.length )
         throw new Error("Missing secret");
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.headers = new Headers([
         ["Content-Type", "application/json; charset=utf-8"]
      ]);
      var body =
         {
            method: "logon",
            secret
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
               return _this._authenticated = json.authenticated;
            }
         )
         .catch(
            function(error) {
               alert(error);
            }
         );

      return promise;
   }
   
   getStatus()
   {
      var _this = this;
      
      this._authenticated = false;
      
      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.headers = new Headers([
         ["Content-Type", "application/json; charset=utf-8"]
      ]);
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
               return response.json()
            }
         )
         .then(
            function(json) {
               _this._authenticated = json.authenticated;
               return _this.authenticated;
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
      
      this._authenticated = false;

      var params = {}
      params.method = "POST";
      params.credentials = "include";
      params.headers = new Headers([
         ["Content-Type", "application/json; charset=utf-8"]
      ]);
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

         
      this._authenticated = false;
      this.secret = null;

      return promise;
   }
   
   get authenticated()
   {
      return this._authenticated;
   }
   
}

var authentication = new Authentication();

function authenticate() {

    authentication.getStatus().then(
        function(authenticated) {
            if (!authenticated) {
               var currentPage = document.location.href;
               var newPage = document.location.origin + "/client/logon/"
               var url = newPage + "?redirect=" + encodeURIComponent(currentPage);
               document.location.href = url
            }
        }
    );
}