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
printf("%d\n",cityCode);
if(cityCode==1)request.forward("Ujjain.html");
else if(cityCode==2)request.forward("Indore.html");
else if(cityCode==3)request.forward("Dewas.html");
else request.forward("index.html");
}
void saveEnquiry(Request &request,Response &response)
{
string n=request.get("nm");
string c=request.get("ct");
string a=request.get("ad");
string m=request.get("msg");
cout<<"data received"<<endl;
cout<<"Name:"<<n<<endl;
cout<<"City:"<<c<<endl;
cout<<"Address:"<<a<<endl;
cout<<"Query:"<<m<<endl;
request.forward("SaveNotification.html");
}
int main()
{
TMWebProjector server(8080);
server.onRequest("/addEnquiry",saveEnquiry);
server.onRequest("/getCity",getCityView);
server.start();
return 0;
}