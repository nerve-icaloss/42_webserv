#ifndef __INCLUDE_SRC_SOCKET_HPP_
#define __INCLUDE_SRC_SOCKET_HPP_

#include <netinet/in.h>
#include <string>
#include <sys/socket.h>
#include <sys/types.h>
#include <vector>

class Socket {

    private:
        int                _serverSocket;
        struct sockaddr_in _serverAddress;
        unsigned int       _port;
        unsigned int       _maxConnection;
        std::string        _ipAddress;
        std::string        _ipClient;
        std::vector<int>   _clientSockets;

    public:
        Socket();
        Socket(std::string ipAddress, unsigned int port, unsigned int maxConnection);
        ~Socket();
        std::string getClientIp();
        void        bindSocket();
        void        listenToConnection();
        void        acceptConnection();
        ssize_t     receiveData(int clientSocket, char *buffer, unsigned int bufferSize);
        void        sendData(int clientSocket, const char *buffer, unsigned int bufferSize);
        void        closeSocket();
        void        closeSocket(int clientSocket);
        void        setSocketTimeout(int clientSocket, int timeoutSec);
};

#endif // !__INCLUDE_SRC_SOCKET_
