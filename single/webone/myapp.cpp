#include<tmwp>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;
using namespace tmwp;

void createErrorPage(Request &request,Response &response)
{
response.write("<!DOCTYPE HTML>");
response.write("<html>");
response.write("<head>");
response.write("<meta charset='utf-8'>");
response.write("<title>Whatever</title>");
response.write("</head>");
response.write("<body>");
response.write("<h1>");
response.write(request.getValue("error"));
response.write("</h1>");
response.write("</body>");
response.write("</html>");
request.forward("index.html");
}
void getCityView(Request &request,Response &response)
{
string cityCodeString=request.get("cityCode");
int cityCode=atoi(cityCodeString.c_str());
request.setKeyValue("error","Invalid choice");
if(cityCode==1)request.forward("Ujjain.html");
else if(cityCode==2)request.forward("Indore.html");
else if(cityCode==3)request.forward("Dewas.html");
else request.forward("errorPage");
}
int main()
{
TMWebProjector server(8080);
server.onRequest("/errorPage",createErrorPage);
server.onRequest("/getCity",getCityView);
server.start();
return 0;
}