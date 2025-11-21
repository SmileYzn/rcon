#pragma once

class CRcon
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void StartFrame();
    static void SendRcon();
    void SendCommand(const char* pszHost, int iPort, const char* pszPassword, const char *pszComand);
    static void CommandResult(const char* pszResult);
private:
   RconProtocol *m_Rcon = nullptr;
};

extern CRcon gRcon;