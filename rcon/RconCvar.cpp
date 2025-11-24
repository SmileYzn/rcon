#include "precompiled.h"

CRconCvar gRconCvar;

void CRconCvar::ServerActivate()
{
    this->m_Host = this->Register("rr_address", "");

    this->m_Port = this->Register("rr_port", "");

    this->m_Pass = this->Register("rr_password", "");
}

cvar_t *CRconCvar::Register(const char *pszName, const char *pszValue)
{
    cvar_t *pPointer = g_engfuncs.pfnCVarGetPointer(pszName);

    if (!pPointer)
    {
        if (pszName)
        {
            if (pszName[0u] != '\0')
            {
                static cvar_t Data;

                Data.name = pszName;

                if (pszValue)
                {
                    if (pszValue[0u] != '\0')
                    {
                        auto Temp = strdup(pszValue);

                        Data.string = Temp;

                        free(Temp);
                    }
                }
                
                Data.flags = (FCVAR_PROTECTED | FCVAR_SPONLY | FCVAR_UNLOGGED);

                g_engfuncs.pfnCVarRegister(&Data);

                pPointer = g_engfuncs.pfnCVarGetPointer(Data.name);

                if (pPointer)
                {
                    g_engfuncs.pfnCvar_DirectSet(pPointer, pszValue);
                }
            }
        }
    }

    return pPointer;
}