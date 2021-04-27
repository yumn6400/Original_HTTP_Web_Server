#include<stdio.h>
#include<tmwp>
#include<string.h>
#include<windows.h>
using namespace tmwp;
typedef struct _request
{
char *method;
char *resource;
char *mimeType;
char isClientSideTechnologyResource;
int dataCount;
char **data;
}REQUEST;
int extensionEquals(const char *left,const char *right)
{
char a,b; 
while(*left&&*right)
{
a=*left;
b=*right;
if(a!=b)return 0;
left++;
right++;
}
return *left==*right;
}
char *isMimeType(char *resource)
{
char *mimeType;
mimeType=NULL;
int i,lastIndexOfDot;
for(i=0;resource[i]!='\0'&&resource[i]!='.';i++);
lastIndexOfDot=i;
if(resource[i]=='\0')return mimeType;
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
for(i=0;bytes[i]!=' '&&bytes[i]!='\0';i++)method[i]=bytes[i];
method[i]='\0';
i+=2;
char **data=NULL;
int dataCount=0;
if(strcmp(method,"GET")==0)
{
//whatever?eeijieh+edi3wo&dijihid&didjgidj&jihffi
for(j=0;bytes[i]!=' ';j++,i++)
{
if(bytes[i]=='?')break;
resource[j]=bytes[i];
}
resource[j]='\0';
if(bytes[i]=='?')
{
i++;
int si=i;
dataCount=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')dataCount++;
i++;
}
dataCount++;
data=(char **)malloc(sizeof(char *)*dataCount);
int *pc=(int *)malloc(sizeof(int)*dataCount);
i=si;
int j=0;
while(bytes[i]!=' ')
{
if(bytes[i]=='&')
{
pc[j]=i;
j++;
}
i++;
}
pc[j]=i;
i=si;
j=0;
int howManyToPick;
while(j<dataCount)
{
howManyToPick=pc[j]-i;
data[j]=(char *)malloc(sizeof(char)*(howManyToPick+1));
strncpy(data[j],bytes+i,howManyToPick);
data[j][howManyToPick]='\0';
i=pc[j]+1;
j++;
}
}
}
printf("Request arrived for %s\n",resource);
REQUEST *request=(REQUEST *)malloc(sizeof(REQUEST));
request->dataCount=dataCount;
request->data=data;
request->method=(char *)malloc((sizeof(char)*strlen(method))+1);
strcpy(request->method,method);
if(resource[0]=='\0')
{
request->resource=NULL;
request->mimeType=NULL;
request->isClientSideTechnologyResource='Y';
}
else
{
request->resource=(char *)malloc((sizeof(char)*strlen(resource))+1);
strcpy(request->resource,resource);
request->isClientSideTechnologyResource=isClientSideResource(request->resource);
request->mimeType=isMimeType(request->resource);
}
return request;
}
TMWebProjector::TMWebProjector(int portNumber)
{
this->portNumber=portNumber;
this->url=NULL;
this->ptrOnRequest=NULL;
}
TMWebProjector::~TMWebProjector()
{
if(this->url)delete []this->url;
}
void TMWebProjector::onRequest(const char *url,void (*ptrOnRequest)(int,char *[]))
{
if(this->url)delete []this->url;
this->url=NULL;
this->ptrOnRequest=NULL;
if(url==NULL||ptrOnRequest==NULL)return;
this->url=(char *)malloc(sizeof(char)*strlen(url));
strcpy(this->url,url);
this->ptrOnRequest=ptrOnRequest;
}
void TMWebProjector::start()
{
FILE *f;
char requestBuffer[8193];
char responseBuffer[1025];
char header[501];
WORD ver;
int success,len,data,i,rc;
int serverSocketDescriptor,clientSocketDescriptor;
struct sockaddr_in serverSocketInformation;
struct sockaddr_in clientSocketInformation;
WSADATA wsaData;
ver=MAKEWORD(1,1);
WSAStartup(ver,&wsaData);
serverSocketDescriptor=socket(AF_INET,SOCK_STREAM,0);
if(serverSocketDescriptor<0)
{
printf("Unable to create a socket");
return;
}
serverSocketInformation.sin_family=AF_INET;
serverSocketInformation.sin_port=htons(this->portNumber);
serverSocketInformation.sin_addr.s_addr=htonl(INADDR_ANY);
success=bind(serverSocketDescriptor,(struct sockaddr *)&serverSocketInformation,sizeof(serverSocketInformation));
if(success<0)
{
char message[101];
sprintf(message,"Unable to bind to port %s",this->portNumber);
printf("%s\n",message);
WSACleanup();
return;
}
while(1)
{
printf("TM web server is ready to accept request \n");
listen(serverSocketDescriptor,10);
len=sizeof(serverSocketInformation);
clientSocketDescriptor=accept(serverSocketDescriptor,(struct sockaddr *)&clientSocketInformation,&len);
if(clientSocketDescriptor<0)
{
char message[101];
sprintf(message,"Unable to accept request from port %s",this->portNumber);
printf("%s\n",message);
closesocket(serverSocketDescriptor);
WSACleanup();
return ;
}
success=recv(clientSocketDescriptor,requestBuffer,sizeof(requestBuffer),0);
printf("request arrived\n");
REQUEST *request=parseRequest(requestBuffer);
printf("Method:%s,Resource:%s,MimeType:%s,ClientSideResource:%c\n",request->method,request->resource,request->mimeType,request->isClientSideTechnologyResource);
if(request->isClientSideTechnologyResource=='Y')
{
if(request->resource==NULL)
{
printf("open index.html\n");
f=fopen("index.html","rb");
if(f==NULL)
{
f=fopen("index.htm","rb");
}
if(f==NULL)
{
char tmp[501];
strcpy(tmp,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /not found</h1></body></html>");
len=strlen(tmp);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",len);
send(clientSocketDescriptor,header,strlen(header)+1,0);
strcpy(responseBuffer,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /not found</h1></body></html>");
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
fseek(f,0,2);
len=ftell(f);
fseek(f,0,0);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\nConnection:close\n\n",len);
send(clientSocketDescriptor,header,strlen(header),0);
i=0;
while(i<len)
{
rc=len-i;
if(rc>1024)rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i=i+rc;
}
fclose(f);
closesocket(clientSocketDescriptor);
}
}
else
{
f=fopen(request->resource,"rb");
if(f==NULL)
{
char tmp[501];
sprintf(tmp,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
len=strlen(tmp);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",len);
send(clientSocketDescriptor,header,strlen(header)+1,0);
sprintf(responseBuffer,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
fseek(f,0,2);
len=ftell(f);
fseek(f,0,0);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:%s\nContent-Length:%d\nConnection:close\n\n",request->mimeType,len);
send(clientSocketDescriptor,header,strlen(header),0);
i=0;
while(i<len)
{
rc=len-i;
if(rc>1024)rc=1024;
fread(&responseBuffer,rc,1,f);
send(clientSocketDescriptor,responseBuffer,rc,0);
i=i+rc;
}
fclose(f);
closesocket(clientSocketDescriptor);
}
}
}
else
{
if(this->url==NULL||this->ptrOnRequest==NULL)
{
char tmp[501];
sprintf(tmp,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
len=strlen(tmp);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",len);
send(clientSocketDescriptor,header,strlen(header)+1,0);
sprintf(responseBuffer,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
int ii=0;
if(this->url[0]=='/')ii=1;
if(strcmp(this->url+ii,request->resource)==0)
{
this->ptrOnRequest(request->dataCount,request->data);
if(request->data!=NULL)
{
for(int k=0;k<request->dataCount;k++)free(request->data[k]);
free(request->data);
}
char tmp[501];
sprintf(tmp,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
len=strlen(tmp);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",len);
send(clientSocketDescriptor,header,strlen(header)+1,0);
sprintf(responseBuffer,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s become processed</h1></body></html>",request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
else
{
char tmp[501];
sprintf(tmp,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
len=strlen(tmp);
sprintf(header,"HTTP/1.1 200 OK\nContent-Type:text/html\nContent-Length:%d\n\n",len);
send(clientSocketDescriptor,header,strlen(header)+1,0);
sprintf(responseBuffer,"<!DOCTYPE html><html lang='en'><head><title>Whatever</title></head><body><h1>Resource /%s not found</h1></body></html>",request->resource);
send(clientSocketDescriptor,responseBuffer,strlen(responseBuffer)+1,0);
}
}
}
}
closesocket(serverSocketDescriptor);
WSACleanup();
}