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
printf("**********\n");
//string cityCodeString=request.get("cityCode");
//int cityCode=atoi(cityCodeString.c_str());
//printf("%d\n",cityCode);
int cityCode=1;
response.write("<!DOCTYPE HTML>");
response.write("<html>");
response.write("<body>");
response.write("<h1>Good</h1>");
response.write("</body>");
response.write("</html>");
request.forward("Ujjain.html");
//else if(cityCode==2)request.forward("Indore123.html");
//else if(cityCode==3)request.forward("Dewas.html");
//else request.forward("errorPage");
//response.close();
}
void createErrorPage(Request &request,Response &response)
{
response.write("<!DOCTYPE HTML>");
response.write("<html>");
response.write("<body>");
response.write("<h1>HEllO</h1>");
response.write("</body>");
response.write("</html>");
request.forward("getCity");
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
request.forward("errorPage");
}
int main()
{
TMWebProjector server(8080);
server.onRequest("/errorPage",createErrorPage);
server.onRequest("/addEnquiry",saveEnquiry);
server.onRequest("/getCity",getCityView);
server.start();
return 0;
}