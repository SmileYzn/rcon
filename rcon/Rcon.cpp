#include "precompiled.h"

CRcon gRcon;

void CRcon::ServerActivate()
{
    static cvar_t tAddress = {"rcon_remote_address", "192.168.100.100", FCVAR_SERVER, 0.0f, NULL};
    g_engfuncs.pfnCVarRegister(&tAddress);
    this->m_Address = g_engfuncs.pfnCVarGetPointer(tAddress.name);

    static cvar_t tPort = {"rcon_remote_port", "27021", FCVAR_SERVER, 0.0f, NULL};
    g_engfuncs.pfnCVarRegister(&tPort);
    this->m_Port = g_engfuncs.pfnCVarGetPointer(tPort.name);

    static cvar_t tPassword = {"rcon_remote_password", "2133", FCVAR_SERVER, 0.0f, NULL};
    g_engfuncs.pfnCVarRegister(&tPassword);
    this->m_Password = g_engfuncs.pfnCVarGetPointer(tPassword.name);

    g_engfuncs.pfnAddServerCommand("rcon_remote_send", this->SendCommand);
}

void CRcon::ServerDeactivate()
{
    /* TEST */
}

void CRcon::StartFrame()
{
    /* PROCESS SOCKET CHANGE HERE */
}

void CRcon::SendCommand()
{
    if (!gRcon.m_Address->string || strlen(gRcon.m_Address->string) < 1)
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] %s is not set.", Plugin_info.logtag, gRcon.m_Address->name);
        return;
    }

    if (!gRcon.m_Port->string || strlen(gRcon.m_Port->string) < 1)
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] %s is not set.", Plugin_info.logtag, gRcon.m_Port->name);
        return;
    }

    if (!gRcon.m_Password->string || strlen(gRcon.m_Password->string) < 1)
    {
        gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] %s is not set.", Plugin_info.logtag, gRcon.m_Password->name);
        return;
    }

    auto pCmdArgs = g_engfuncs.pfnCmd_Args();

    if (pCmdArgs)
    {
        if (pCmdArgs[0u] != '\0')
        {
            int iError = 0;

            int iSocket = gRconSocket.Open(gRcon.m_Address->string, gRcon.m_Port->string, SOCKET_UDP, &iError, 0);
            
            if (iError)
            {
                gRconSocket.Close(iSocket);
                gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Error %d creating the socket.", Plugin_info.logtag, iError);
                return;
            }
            
            char iSend[256] = {0};
            char szCmd[256] = {0};
            char szRcon[32] = {0};
            char szNone[64] = {0};
            
            snprintf(iSend, sizeof(iSend), "%c%c%c%cchallenge rcon",255,255,255,255);

            gRconSocket.Send2(iSocket, iSend, strlen(iSend));
            
            if(!gRconSocket.IsReadable(iSocket, 1000000)) // If socket not change in 2 seconds, then stop it
            {
                gRconSocket.Close(iSocket);

                gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] No response from %s:%s", Plugin_info.logtag, gRcon.m_Address->string, gRcon.m_Port->string);
                return;
            }

            gRconSocket.Recv(iSocket, szCmd, sizeof(szCmd));

            LOG_CONSOLE(PLID, "[TEST] %s", szCmd);

            //parse(szCmd, szNone, charsmax(szNone), szNone, charsmax(szNone), szRcon, charsmax(szRcon));
            
            // snprintf (szCmd,sizeof(szCmd), "%c%c%c%crcon %s \"%s\" %s",255,255,255,255, szRcon, gRcon.m_Password->string, szCommand);

            // socket_send2(iSocket,szCmd,charsmax(szCmd));
            
            // if(socket_change(iSocket))
            // {
            //     new szBuffer[2048];
            //     socket_recv(iSocket,szBuffer,charsmax(szBuffer));

            //     server_print(szBuffer);
            // }

            // socket_close(iSocket);
            return;
        }
    }

    gpMetaUtilFuncs->pfnLogConsole(PLID, "[%s] Command Usage: %s <command>", Plugin_info.logtag, g_engfuncs.pfnCmd_Argv(0));
}