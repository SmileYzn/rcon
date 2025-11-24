#pragma once

class CRcon
{
public:
    void ServerActivate();
    void StartFrame();
    static void SendRcon();
    void SendCommand(const char* pszHost, int iPort, const char* pszPassword, const char *pszComand);

private:
    RconProtocol* m_Rcon;
};

extern CRcon gRcon;