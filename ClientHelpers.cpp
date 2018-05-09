#include "CompilerCfg.hpp"
#ifdef _CLIENT
#include "net/Netty.hpp"
#include "net/winsock2.h"
#include "Config.hpp"
#include <cstdlib>
#include <memory>

using std::shared_ptr;

shared_ptr<Client> client(nullptr);
shared_ptr<net::ClientBinding> binding;
shared_ptr<net::Connection> conn;

Client& getClient(){
	if(client.get()==nullptr){
		binding.reset();
		conn.reset();
		SOCKET out;
		Configuration cfg;
		string addr = cfg.getHostname();
		sockaddr_in inetSocketAddress;
		inetSocketAddress.sin_family = IN_ADDR;
		inetSocketAddress.sin_port = (short)cfg.getPort();
		inetSocketAddress.sin_addr.S_un = inet_addr(addr.c_str());
		out = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
		//connect(out,(sockaddr*)&inetSocketAddress,sizeof(sockaddr_in));
		conn = std::make_shared<net::Connection>(out);
		binding = std::make_shared<net::ClientBinding>(*conn);
		client = std::make_shared<Client>(*binding);
	}
	return *client;
}


#endif
