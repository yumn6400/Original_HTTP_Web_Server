#include<tmwp>
#include<iostream>
using namespace std;
using namespace tmwp;
void doSomething()
{
cout<<"Great,this code get executed on server side"<<endl;
}
int main()
{
TMWebProjector server(5050);
server.onRequest("/kkk",doSomething);
server.start();
return 0;
}
