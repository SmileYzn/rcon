#pragma once

#define STATE_NONE 0
#define STATE_CHALLENGE 1
#define STATE_COMAND 2
#define STATE_CLOSE 3

class CRcon
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void StartFrame();

    void SendCommand(const char* pszCommand);

    int  m_Socket;
    int  m_Error;
    int  m_State;

    char m_Send[512];
    char m_Data[256];
    char m_Numb[64];
};

extern CRcon gRcon;