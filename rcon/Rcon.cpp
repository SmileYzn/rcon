#include "precompiled.h"

CRcon gRcon;

void CRcon::ServerActivate()
{
    this->m_Socket = 0;
    this->m_Error = 0;
    this->m_State = STATE_NONE;

    Q_memset(this->m_Send, 0, sizeof(this->m_Send));
    Q_memset(this->m_Data, 0, sizeof(this->m_Data));
    Q_memset(this->m_Numb, 0, sizeof(this->m_Numb));

    this->SendCommand("status");
}

void CRcon::ServerDeactivate()
{
    if (this->m_Socket)
    {
        gRconSocket.Close(this->m_Socket);
    }
    
    this->m_Socket = 0;
    this->m_Error = 0;
}

void CRcon::StartFrame()
{
    switch (this->m_State)
    {
        case STATE_CHALLENGE:
        {
            if(gRconSocket.IsReadable(this->m_Socket, 100000)) 
            {
                gRconSocket.Recv(this->m_Socket, this->m_Numb, sizeof(this->m_Numb));

                char *p = strstr(this->m_Numb, "challenge ");

                int len = strlen("challenge ");

                Q_memmove(p, p + len, strlen(p + len) + 1);

                this->m_State = STATE_COMAND;
            }
            break;
        }
        case STATE_COMAND:
        {
            snprintf(this->m_Send, sizeof(this->m_Send), "%s \"%s\" %s", this->m_Numb, "2133", this->m_Data);
            //LOG_CONSOLE(PLID, "TESTE %s", this->m_Send);
            //this->m_State = STATE_CLOSE;
            gRconSocket.Send2(this->m_Socket, this->m_Send, strlen(this->m_Send));
            break;
        }
        case STATE_CLOSE:
        {
            if (this->m_Socket)
            {
                gRconSocket.Close(this->m_Socket);
            }
            
            this->m_Socket = 0;
            this->m_Error = 0;
            break;
        }
    }
}

void CRcon::SendCommand(const char* pszCommand)
{
    if (pszCommand)
    {
        if (pszCommand[0u] != '\0')
        {
            if (this->m_State == STATE_NONE)
            {
                Q_strncpy(this->m_Data, pszCommand, sizeof(this->m_Data));

                this->m_Socket = gRconSocket.Open("192.168.100.100", "27021", SOCKET_UDP, &this->m_Error, 0);

                if (this->m_Error != 0)
                {
                    gRconSocket.Close(this->m_Socket);
                    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Error %d creating the socket.", Plugin_info.logtag, this->m_Error);
                    return;
                }

                this->m_State = STATE_CHALLENGE;

                snprintf(this->m_Send, sizeof(this->m_Send), "%c%c%c%cchallenge rcon", 255, 255, 255, 255);

                gRconSocket.Send2(this->m_Socket, this->m_Send, strlen(this->m_Send));
            }
        }
    }
}