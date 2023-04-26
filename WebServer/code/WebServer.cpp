#include <iostream>
#include "WebServer.hpp"

int main(int argc, char** argv) {

   using namespace std;
   using namespace BeeFish;

   cout << "WebServer " << WEB_SERVER << endl;

   WebServer webServer(80);

   webServer.start();

   return 0;
}