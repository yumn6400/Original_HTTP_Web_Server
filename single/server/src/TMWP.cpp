#include<tmwp>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<windows.h>
using namespace tmwp;
typedef struct _request
{
char *method;
char *resource;
char *MIMEType;
char isClientSideTechnologyResource;
}REQUEST;
int extensionEquals(const char *left,const char *right)
{
char a,b;
while(*left&&*right)
{
a=*left;
b=*right;
if(a>=65&&a<=90)a=a+32;
if(b>=65&&b<=90)b=b+32;
if(a!=b)return 0;
left++;
right++;
}
return *left==*right;
}
char *getMimeType(char *resource)
{
char *mimeType;
mimeType=NULL;
int len=strlen(resource);
if(len<4)return mimeType;
int lastIndexOfDot=len-1;
for(;lastIndexOfDot>0&&resource[lastIndexOfDot]!='.';lastIndexOfDot--);
if(lastIndexOfDot==0)return mimeType;
if(extensionEquals(resource+lastIndexOfDot+1,"html"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"text/html");
}
if(extensionEquals(resource+lastIndexOfDot+1,"css"))
{
mimeType=(char *)malloc(sizeof(char)*9);
strcpy(mimeType,"text/css");
}
if(extensionEquals(resource+lastIndexOfDot+1,"js"))
{
mimeType=(char *)malloc(sizeof(char)*16);
strcpy(mimeType,"text/javascript");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals(resource+lastIndexOfDot+1,"jpeg"))
{
mimeType=(char *)malloc(sizeof(char)*11);
strcpy(mimeType,"image/jpeg");
}
if(extensionEquals(resource+lastIndexOfDot+1,"png"))
{
mimeType=(char *)malloc(sizeof(char)*10);
strcpy(mimeType,"image/png");
}
if(extensionEquals(resource+lastIndexOfDot+1,"ico"))
{
mimeType=(char *)malloc(sizeof(char)*13);
strcpy(mimeType,"image/x-icon");
}
return mimeType;
}
char isClientSideResource(char *resource)
{
int i;
for(i=0;resource[i]!='\0'&&resource[i]!='.';i++);
if(resource[i]=='\0')return 'N';
return 'Y';
}
REQUEST *parseRequest(char *bytes)
{
char method[11];
char resource[1001];
int i,j;
for(i=0;bytes[i]!=' ';i++)method[i]=bytes[i];
method[i]='\0';
i+=2;
for(j=0;bytes[i]!=' ';i++,j++)resource[j]=bytes[i];
resource[j]='\0';
REQUEST *request=(REQUEST *)malloc(sizeof(REQUEST));
request->method=(char *)malloc(sizeof(char)*strlen(method)+1);
strcpy(request->method,method);
if(resource[0]=='\0')
{
request->resource=NULL;
request->MIMEType=NULL;
request->isClientSideTechnologyResource='Y';
}
else
{
request->resource=(char *)malloc(sizeof(char)*strlen(resource)+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(resource);
request->MIMEType=getMimeType(resource);
}
return request;
}
TMWebProjector::TMWebProjector(int portNumber)
{
this->portNumber=portNumber;
this->url=url;
this->ptrOnRequest=NULL;
}
TMWebProjector::~TMWebProjector()
{
if(this->url)delete []this->url;
}
void TMWebProjector::onRequest(const char *url,void (*ptrOnRequest)(void))
{
if(this->url)delete []this->url;
this->url=NULL;
this->ptrOnRequest=NULL;
if(url==NULL||ptrOnRequest==NULL)return;
this->url=new char[strlen(url)+1];
strcpy(this->url,url);
this->ptrOnRequest=ptrOnRequest;
}
void TMWebProjector::start()
{
FILE *f;
char requestBuffer[8193];
char responseBuffer[1025];
int serverSocketDescriptor;
int clientSocketDescriptor;
int success,len,byteExtracted,i,length,g,rc;
struct sockaddr_in clientSocketInformation;
struct sockaddr_in serverSocketInformation;
WORD ver;
ver=MAKEWORD(1,1);
WSADATA wsaData;
WSAStartup(ver,&wsaData);
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to create a socket\n");
return ;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
clientSocketDescriptor=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
if(clientSocketDescriptor<0)
{
printf("Unable to bind to port5050\n");
WSACleanup();
return ;
}
listen(serverSocketDescriptor,10);
while(1)
{
printf("TM server is ready to accept request on port 5050\n");
len=sizeof(clientSocketInformation);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
printf("Unable to accept request to port5050\n");
closesocket(serverSocketDescriptor);
WSACleanup();
return ;
}
byteExtracted=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
if(byteExtracted<0)
{
printf("byte extracted less than 0");
}else
if(byteExtracted==0)
{
printf("Byte extracted is equal to 0");
}
else
{
requestBuffer[byteExtracted]='\0';
REQUEST *request=parseRequest(requestBuffer);
printf("---%s\n",request->resource);
if(request->isClientSideTechnologyResource=='Y')
{
if(request->resource==NULL)
{
printf("index.html\n");
f=fopen("index.html","rb");
if(f==NULL)
{
printf("index.htm\n");
f=fopen("index.htm","rb");
}
if(f==NULL)
{
printf("not open any html\n");
strcpy(responseBuffer,"<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>");
len=strlen(responseBuffer);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>",len);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
printf("open any one of index\n");
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nkeep-Alive:timeout=5,max=1000\n\n",length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024)rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}//while loop ends
fclose(f);
closesocket(clientSocketDescriptor);
}//else condition ends
}//request->resource ends
else
{
printf("%s\n",request->resource);
f=fopen(request->resource,"rb");
if(f==NULL)
{
char tmp[501];
strcpy(tmp,"<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>");
len=strlen(tmp);
printf("%d\n",len);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>",len);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
fseek(f,0,2);
length=ftell(f);
fseek(f,0,0);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nkeep-Alive:timeout=5,max=1000\n\n",request->MIMEType,length);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer),0);
printf("-------%s\n",request->MIMEType);
i=0;
while(i<length)
{
rc=length-i;
if(rc>1024)rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i+=rc;
}//while loop ends
fclose(f);
closesocket(clientSocketDescriptor);
}//else condition ends
}//above else condition ends
}//request->isClientSideTechnologyResources ends
else
{
if(this->url==NULL||this->ptrOnRequest==NULL)
{
printf("Sending 404 page\n");
char tmp[501];
strcpy(tmp,"<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>");
len=strlen(tmp);
printf("%d\n",len);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /%s not found</h1></body></html>",len,request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
int ii=0;
if(this->url[0]=='/')ii=1;
if(strcmp(this->url+ii,request->resource)==0)
{
this->ptrOnRequest();
printf("Sending processed page\n");
char tmp[501];
strcpy(tmp,"<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>");
len=strlen(tmp);
printf("%d\n",len);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /%s processed</h1></body></html>",len,request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
char tmp[501];
strcpy(tmp,"<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /not found</h1></body></html>");
len=strlen(tmp);
printf("%d\n",len);
sprintf(responseBuffer,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n<!DOCTYPE html><html lang='en'><head><meta charset='utf-8'><title>Whatever</title></head><body><h1 style='color:red'>Resources /%s not found</h1></body></html>",len,request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
}
}
}//else condition ends
}
closesocket(serverSocketDescriptor);
WSACleanup();
return ;
}