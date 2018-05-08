#include "CompilerCfg.hpp"
#ifdef _CLIENT
#include "net/Netty.hpp"
#include "net/winsock2.h"
#include "Config.hpp"
#include <cstdlib>
Client* client(nullptr);
net::ClientBinding* binding;
net::Connection* conn;

Client& getClient(){
	if(client==nullptr){
		SOCKET out;
		Configuration cfg;
		string addr = cfg.getHostname();
		sockaddr_in inetSocketAddress;
		inetSocketAddress.sin_family = IN_ADDR;
		inetSocketAddress.sin_port = (short)cfg.getPort();
		inetSocketAddress.sin_addr.S_un = inet_addr(addr.c_str());
		out = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		//connect(out,(sockaddr*)&inetSocketAddress,sizeof(sockaddr_in));
		conn = new net::Connection(out);
		binding = new net::ClientBinding(*conn);
		client = new Client(*binding);
		atExit(exitHandler);
	}
	return *client;
}

void exitHandler(int i){
	delete client;
	delete binding;
	delete conn;
}

#endif
