#include "precompiled.h"

CRconSocket gRconSocket;

void CRconSocket::ServerActivate()
{
#ifdef _WIN32
	WSADATA WSAData;
	int errorcode = WSAStartup(MAKEWORD(2, 2), &WSAData);

	if (errorcode != 0)
	{
		MF_Log("[%s]: WSAStartup failed with error code %d. Natives will not be available.", MODULE_LOGTAG, errorcode);
		return;
	}

	this->m_Winsock_Initialized = true;
#endif
}

void CRconSocket::ServerDeactivate()
{
#ifdef _WIN32
	if (this->m_Winsock_Initialized)
	{
		WSACleanup();
	}
#endif
	delete[] this->m_Send2_Buffer;
}

bool CRconSocket::SetNonBlocking(int Socket)
{
#ifdef _WIN32
	unsigned long flags = 1;
	return (ioctlsocket(Socket, FIONBIO, &flags) == 0);
#else
	int flags = -1;

	if ((flags = fcntl(Socket, F_GETFL, 0)) == -1)
	{
		return false;
	}

	if (fcntl(Socket, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		return false;
	}
	return true;
#endif
}

int CRconSocket::Open(const char *Hostname, const char *Port, int Protocol, int *ErrorFlags, int Flags)
{
	*ErrorFlags = 0;

	bool nonblocking_socket = (Flags & SOCK_NON_BLOCKING) != 0;

	bool libc_errors = (Flags & SOCK_LIBC_ERRORS) != 0;

	if (!Hostname || strlen(Hostname) < 1)
	{
		*ErrorFlags = libc_errors ? SOCK_ERROR_EHOSTUNREACH : SOCK_ERROR_SERVER_UNKNOWN;
		return -1;
	}

	int sockfd = -1, getaddrinfo_status = -1, connect_status = -1;

	bool setnonblocking_status = false, connect_inprogress = false;

	struct addrinfo hints, *server_info, *server;

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = Protocol;

	if ((getaddrinfo_status = getaddrinfo(Hostname, Port, &hints, &server_info)) != 0)
	{
		*ErrorFlags = libc_errors ? getaddrinfo_status : SOCK_ERROR_SERVER_UNKNOWN;
		return -1;
	}

	server = server_info;

	do
	{
		if ((sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol)) != -1)
		{
			if (nonblocking_socket)
			{
				setnonblocking_status = this->SetNonBlocking(sockfd);
			}

			if (nonblocking_socket == false || (nonblocking_socket && setnonblocking_status == true))
			{
				if ((connect_status = connect(sockfd, server->ai_addr, server->ai_addrlen)) == -1)
				{
					*ErrorFlags = libc_errors ? errno : SOCK_ERROR_WHILE_CONNECTING;

					if (nonblocking_socket && (errno == EINPROGRESS || errno == EWOULDBLOCK))
					{
						connect_inprogress = true;
					}
					else
					{
						close(sockfd);
					}
				}
				else
				{
					*ErrorFlags = 0;
				}
			}
			else
			{
				if (*ErrorFlags == 0)
				{
					*ErrorFlags = libc_errors ? errno : SOCK_ERROR_CREATE_SOCKET;
				}
			}
		}
		else
		{
			if (*ErrorFlags == 0)
			{
				*ErrorFlags = errno;
			}
		}

	} while ((nonblocking_socket && connect_inprogress == false) && connect_status != 0 && (server = server->ai_next) != nullptr);

	freeaddrinfo(server_info);

	if (sockfd == -1 || server == nullptr)
	{
		return -1;
	}

	return sockfd;
}

int CRconSocket::Close(int SocketForward)
{
	return (close(SocketForward) == -1) ? 0 : 1;
}

int CRconSocket::Recv(int SocketForward, char *Data, int Length)
{
	char *recv_buffer = new char[Length];

	if (recv_buffer == nullptr)
	{
		return -1;
	}

	memset(recv_buffer, 0, Length);

	int bytes_received = -1;

	bytes_received = recv(SocketForward, recv_buffer, Length - 1, 0);

	if (bytes_received == -1)
	{
		delete[] recv_buffer;
		return -1;
	}

	recv_buffer[bytes_received] = '\0';

	char *destination = Data;

	int current_length = 0;
	int max_length = Length - 1;

	const char *buffer = recv_buffer;

	while (max_length-- && current_length < bytes_received)
	{
		*destination++ = (char)*buffer++;
		current_length++;
	}

	*destination = 0;

	delete[] recv_buffer;

	return bytes_received;
}

// native socket_send(_socket, _data[], _length);
int CRconSocket::Send(int SocketForward, char *Data, int Length)
{
	return send(SocketForward, Data, Length, 0);
}

// native socket_send2(_socket, _data[], _length);
int CRconSocket::Send2(int SocketForward, char *Data, int Length)
{
	int SendLength = Length;

	if (SendLength > this->m_Send2_Buffer_Length)
	{
		delete[] this->m_Send2_Buffer;

		this->m_Send2_Buffer = new char[SendLength + 1];

		if (this->m_Send2_Buffer == nullptr)
		{
			this->m_Send2_Buffer_Length = 0;
			return -1;
		}

		this->m_Send2_Buffer_Length = SendLength;
	}

	char *data = Data;

	char *buffer = this->m_Send2_Buffer;

	while (SendLength--)
	{
		*buffer++ = (char)*data++;
	}

	return send(SocketForward, this->m_Send2_Buffer, Length, 0);
}

// native socket_is_readable(_socket, _timeout = 100000);
int CRconSocket::IsReadable(int SocketForward, unsigned int Timeout)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = Timeout;

	fd_set readfds;
	FD_ZERO(&readfds);
	FD_SET(SocketForward, &readfds);

	return (select(SocketForward + 1, &readfds, nullptr, nullptr, &tv) > 0) ? 1 : 0;
}

// native socket_is_writable(_socket, _timeout = 100000);
int CRconSocket::IsWritable(int SocketForward, unsigned int Timeout)
{
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = Timeout;

	fd_set writefds;
	FD_ZERO(&writefds);
	FD_SET(SocketForward, &writefds);

	return (select(SocketForward + 1, nullptr, &writefds, nullptr, &tv) > 0) ? 1 : 0;
}
