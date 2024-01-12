#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <iostream>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <string>

int main()
{
	// create a socket
	int soc = socket(AF_INET, SOCK_STREAM, 0);
	if (soc == -1)
	{
		std::cerr << "socket error" << std::endl;
		return (1);
	}
	// creata a hint struct for the server we're connecting with
	int port = 123;
	std::string ipAdress = "127.0.0.1";

	struct sockaddr_in hint;
	hint.sin_family = AF_INET;
	hint.sin_port = htons(port);
	inet_pton(AF_INET, ipAdress.c_str(), &hint.sin_addr);
	// connect to the server on the socket
	int connectResult = connect(soc, (struct sockaddr *)&hint, sizeof(hint));
	if (connectResult == -1)
	{
		std::cerr << "connect error" << std::endl;
		return (2);
	}
	// while loop:
	char buf[4096];
	std::string userInput;

	while (1)
	{
		// enter lines of text
		std::cout << "> ";
		getline(std::cin, userInput);
		// send to server
		int sendResult = send(soc, userInput.c_str(), userInput.size() + 1, 0);
		if (sendResult == -1)
			std::cout << "send error" << std::endl;
		int bytesReceived = recv(soc, buf, 4096, MSG_DONTWAIT);
		// else
		// {
		// 	// wait for response
		// 	memset(buf, 0, 4096);
		// 	int bytesReceived = recv(soc, buf, 4096, 0);
		// 	if (bytesReceived == -1)
		// 	{
		// 		std::cerr << "connection issue" << std::endl;
		// 		break ;
		// 	}
		// 	if (bytesReceived == 0)
		// 	{
		// 		std::cout << "server disconnected" << std::endl;
		// 		break ;
		// 	}
		// 	// display response
		// 	std::cout << "SERVER> " << std::string(buf, bytesReceived) << std::endl;
		// }
	}
	// close socket
	close(soc);
	return (0);
}