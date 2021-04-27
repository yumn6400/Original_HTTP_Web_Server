#include<windows.h>
#include<stdio.h>
#include<string.h>
int main()
{
char response[5000];
char request[5000];
char header[5000];
char responseLength[11];
int successCode,len;
int serverSocketDescriptor;
int clientSocketDescriptor;
struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;
WORD ver;
WSADATA wsaData;
ver=MAKEWORD(1,1);
WSAStartup(ver,&wsaData);
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to connect to socket\n");
return 0;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(5050);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
successCode=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
if(successCode<0)
{
printf("Unable to bind to port 5050\n");
WSACleanup();
return 0;
}
printf("TMServer is ready to accept request on port 5050\n");
listen(serverSocketDescriptor,10);
len=sizeof(serverSocketInformation);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("Unable to connect to port 5050\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return 0;
}
successCode=recv(clientSocketDescriptor,request,sizeof(request),0);
if(successCode>0)
{
printf("Request arrived\n");
printf("%s\n",request);
}
strcpy(response,"<!DOCTYPE html>");
strcat(response,"<html lang='en'>");
strcat(response,"<head>");
strcat(response,"<meta charset='utf-8'>");
strcat(response,"<title>whatever</title>");
strcat(response,"</head>");
strcat(response,"<body>");
strcat(response,"<h1>Hello,My name is pradhyumn Patidar ,I live in borda village I study in Ujjain Enginnering college,Ujjain,My hobby is to sleep,read</h1>");
strcat(response,"</body>");
strcat(response,"</html>");
strcpy(header,"HTTP/1.1 200 OK\n");
strcat(header,"Content-Type:text/html\n");
strcat(header,"Content-Length:");
sprintf(responseLength,"%d\n",strlen(response)+1);
strcat(header,responseLength);
strcat(header,"\n");
successCode=send(clientSocketDescriptor,header,strlen(header)+1,0);
successCode=send(clientSocketDescriptor,response,strlen(response)+1,0);
if(successCode>0)
{
printf("Response sent\n");
}
closesocket(serverSocketDescriptor);
closesocket(clientSocketDescriptor);
WSACleanup();
return 0;
}