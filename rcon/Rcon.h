#pragma once

class CRcon
{
public:
    void ServerActivate();
    void ServerDeactivate();
    void StartFrame();
    
    static void SendCommand();

    cvar_t* m_Address = nullptr;
    cvar_t* m_Port = nullptr;
    cvar_t* m_Password = nullptr;
};

extern CRcon gRcon;