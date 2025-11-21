#include "precompiled.h"

CRcon gRcon;

void CRcon::ServerActivate()
{
    this->m_Rcon = nullptr;

    g_engfuncs.pfnAddServerCommand("rr_send", this->SendRcon);
}

void CRcon::ServerDeactivate()
{
    if (this->m_Rcon)
    {
        delete this->m_Rcon;

        this->m_Rcon = nullptr;
    }
}

void CRcon::StartFrame()
{
    if (this->m_Rcon)
    {
        this->m_Rcon->Frame();

        std::string Result = this->m_Rcon->GetResult();

        if (!Result.empty())
        {
            LOG_CONSOLE(PLID, "%s", Result.c_str());

            delete this->m_Rcon;
        }
    }
}

void CRcon::SendRcon()
{
    if (g_engfuncs.pfnCmd_Argc() >= 2)
    {
        auto pCmdArgs = g_engfuncs.pfnCmd_Args();

        if (pCmdArgs)
        {
            if (pCmdArgs[0u] != '\0')
            {
                if (gRconCvar.m_Host->string[0u] == '\0')
                {
                    LOG_CONSOLE(PLID, "[%s] Fill the '%s' console variable.", Plugin_info.logtag, gRconCvar.m_Host->name);
                    return;
                }

                if (gRconCvar.m_Port->string[0u] == '\0')
                {
                    LOG_CONSOLE(PLID, "[%s] Fill the '%s' console variable.", Plugin_info.logtag, gRconCvar.m_Port->name);
                    return;
                }

                if (gRconCvar.m_Pass->string[0u] == '\0')
                {
                    LOG_CONSOLE(PLID, "[%s] Fill the '%s' console variable.", Plugin_info.logtag, gRconCvar.m_Pass->name);
                    return;
                }

                gRcon.SendCommand(gRconCvar.m_Host->string, gRconCvar.m_Port->value, gRconCvar.m_Pass->string, pCmdArgs);

                return;
            }
        }
    }
    
    LOG_CONSOLE(PLID, "[%s] Usage: ^3%s^1 <command>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
}

void CRcon::SendCommand(const char* pszHost, int iPort, const char* pszPassword, const char *pszComand)
{
    if (!this->m_Rcon)
    {
        this->m_Rcon = new RconProtocol(pszPassword);

        this->m_Rcon->Connect(pszHost, iPort);

        this->m_Rcon->Send(pszComand);
    }
}
