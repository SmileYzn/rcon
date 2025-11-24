#include "precompiled.h"

CRcon gRcon;

void CRcon::ServerActivate()
{
    char szCommand[] = "rr_send";
    g_engfuncs.pfnAddServerCommand(szCommand, this->SendRcon);
}

void CRcon::StartFrame()
{
    if (this->m_Rcon)
    {
        auto lpResult = this->m_Rcon->GetResult();

        if (lpResult)
        {
            if (lpResult[0u] != '\0')
            {
                LOG_CONSOLE(PLID, "%s", lpResult);

                delete this->m_Rcon;

                this->m_Rcon = nullptr;
            }
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
    
    LOG_CONSOLE(PLID, "[%s] Usage: '%s' <command>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
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
