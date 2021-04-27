#include<tmwp>
#include<stdio.h>
#include<fstream>
#include<iostream>
#include<stdlib.h>
#include<string.h>
using namespace std;
using namespace tmwp;
void getCityView(Request &request,Response &response)
{
string cityCodeString=request.get("cityCode");
int cityCode=atoi(cityCodeString.c_str());
if(cityCode==1)request.forward("Ujjain.html");
else if(cityCode==2)request.forward("Indore123.html");
else if(cityCode==3)request.forward("Dewas.html");
else request.forward("index.html");
}
int main()
{
TMWebProjector server(8080);
server.onRequest("/getCity",getCityView);
server.start();
return 0;
}