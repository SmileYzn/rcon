#pragma once

class CRconCvar
{
public:
    void ServerActivate();
    
    cvar_t *Register(const char *pszName, const char *pszValue);

    cvar_t *m_Host;
    cvar_t *m_Port;
    cvar_t *m_Pass;
};

extern CRconCvar gRconCvar;