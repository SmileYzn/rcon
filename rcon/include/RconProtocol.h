#pragma once

constexpr auto RCON_STATE_NONE = 0;
constexpr auto RCON_STATE_CHALLENGE = 1;
constexpr auto RCON_STATE_COMMAND = 2;

class RconProtocol
{
public:
    RconProtocol(std::string Password)
    {
        this->m_iSocket = -1;

        this->m_iState = RCON_STATE_NONE;
        
        this->m_Password = Password;

        Q_memset(&this->m_sSockAddr, 0, sizeof(this->m_sSockAddr));

        this->m_ResultFunction = nullptr;
    }

    virtual ~RconProtocol()
    {
        if (this->m_iSocket >= 0)
        {
            close(this->m_iSocket);

            this->m_iSocket = -1;

            this->m_iState = RCON_STATE_NONE;

            Q_memset(&this->m_sSockAddr, 0, sizeof(this->m_sSockAddr));

            this->m_ResultFunction = nullptr;
        }
    }

    bool Connect(const char* pszHost, int iPort)
    {
        if (this->m_iSocket < 0)
        {
            if (pszHost)
            {
                this->m_iSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

                if (this->m_iSocket >= 0)
                {
                    int Flags = fcntl(this->m_iSocket, F_GETFL, 0);

                    fcntl(this->m_iSocket, F_SETFL, Flags | O_NONBLOCK);

                    this->m_sSockAddr.sin_family = AF_INET;
                    this->m_sSockAddr.sin_port = htons(iPort);

                    inet_pton(AF_INET, pszHost, &this->m_sSockAddr.sin_addr);

                    return true;
                }
            }
        }

        return false;
    }

    bool Send(std::string Command, void *ResultFunction)
    {
        if (this->m_iSocket >= 0)
        {
            if (!Command.empty())
            {
                this->m_Command = Command;

                this->m_ResultFunction = ResultFunction;

                if (this->m_iState == RCON_STATE_NONE)
                {
                    if (this->m_Challenge.empty())
                    {
                        char szData[24] = {0};
                        Q_snprintf(szData, sizeof(szData), "%c%c%c%cchallenge rcon", 0xFF, 0xFF, 0xFF, 0xFF);

                        sendto(this->m_iSocket, szData, strlen(szData), 0, (sockaddr*)(&this->m_sSockAddr), sizeof(this->m_sSockAddr));

                        this->m_iState = RCON_STATE_CHALLENGE;
                    }
                    else
                    {
                        this->m_iState = RCON_STATE_COMMAND;

                        char szData[512] = {0};
                        Q_snprintf(szData, sizeof(szData), "%c%c%c%crcon %s \"%s\" %s", 0xFF, 0xFF, 0xFF, 0xFF, this->m_Challenge.c_str(), this->m_Password.c_str(), this->m_Command.c_str());

                        sendto(this->m_iSocket, szData, strlen(szData), 0, (sockaddr*)(&this->m_sSockAddr), sizeof(this->m_sSockAddr));
                    }

                    return true;
                }
            }
        }

        return false;
    }

    void Frame()
    {
        if (this->m_iSocket >= 0)
        {
            if (this->m_iState != RCON_STATE_NONE)
            {
                char szBuffer[8192] = {0};

                Q_memset(szBuffer, '\0', sizeof(szBuffer));

                sockaddr_in sAddrInput;

                socklen_t sAddrInputLength = sizeof(sAddrInput);

                while (true)
                {
                    int Received = recvfrom(this->m_iSocket, szBuffer, sizeof(szBuffer) - 1, 0, (sockaddr*)(&sAddrInput), &sAddrInputLength);

                    if (Received < 0)
                    {
                        if (errno == EWOULDBLOCK || errno == EAGAIN)
                        {
                            // Nothing to read, keep going
                        }
                        return;
                    }

                    szBuffer[Received] = '\0';
                    
                    if (this->m_iState == RCON_STATE_CHALLENGE)
                    {
                        this->m_iState = RCON_STATE_NONE;

                        this->m_Challenge = szBuffer;

                        this->m_Challenge.erase(0, 19);

                        this->m_Challenge[this->m_Challenge.size() - 1] = '\0'; 

                        this->Send(this->m_Command, this->m_ResultFunction);
                    }
                    else if (this->m_iState == RCON_STATE_COMMAND)
                    {
                        if (this->m_ResultFunction)
                        {
                            ((void(*)(const char*))this->m_ResultFunction)(szBuffer);
                        }
                        else
                        {
                            LOG_CONSOLE(PLID, "%s", szBuffer);
                        }

                        this->m_iState = RCON_STATE_NONE;
                    }
                }
            }
        }
    }

private:
    int m_iSocket = -1;
    int m_iState  = RCON_STATE_NONE;
    sockaddr_in m_sSockAddr;

    std::string m_Password;
    std::string m_Command;
    std::string m_Challenge;

   void *m_ResultFunction;
};

