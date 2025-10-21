#pragma once

enum RCON_SOCKET_TYPE
{
    SOCKET_TCP = 1,
    SOCKET_UDP = 2
};

enum RCON_SOCKET_FLAGS
{
    SOCK_NON_BLOCKING = (1 << 0), /* Set the socket a nonblocking */
    SOCK_LIBC_ERRORS = (1 << 1)   /* Enable libc error reporting */
};

enum RCON_SOCKET_ERROR
{
    SOCK_ERROR_OK = 0,              /* No error */
    SOCK_ERROR_CREATE_SOCKET = 1,   /* Couldn't create a socket */
    SOCK_ERROR_SERVER_UNKNOWN = 2,  /* Server unknown */
    SOCK_ERROR_WHILE_CONNECTING = 3, /* Error while connecting */
    SOCK_ERROR_EHOSTUNREACH = 113, /* libc error code: No route to host */
};

class CRconSocket
{
public:
    void ServerActivate();
    void ServerDeactivate();

    // native socket_set_non_blocking(_socket);
    bool SetNonBlocking(int SocketForward);

    // native socket_open(_hostname[], _port, _protocol = SOCKET_TCP, &_error, _flags = 0);
    int Open(const char *Hostname, const char *Port, int Protocol, int *ErrorFlags, int Flags);

    // native socket_close(_socket);
    int Close(int SocketForward);

    // native socket_recv(_socket, _data[], _length);
    int Recv(int SocketForward, char* Data, int Length);

    // native socket_send(_socket, _data[], _length);
    int Send(int SocketForward, char* Data, int Length);

    // native socket_send2(_socket, _data[], _length);
    int Send2(int SocketForward, char* Data, int Length);

    // native socket_is_readable(_socket, _timeout = 100000);
    int IsReadable(int SocketForward, unsigned int Timeout);

    // native socket_is_writable(_socket, _timeout = 100000);
    int IsWritable(int SocketForward, unsigned int Timeout);

private:
    bool m_Winsock_Initialized = false;
    char *m_Send2_Buffer = nullptr;
    int m_Send2_Buffer_Length = 0;
};

extern CRconSocket gRconSocket;