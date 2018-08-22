/**
 * vim: set ts=4 :
 * =============================================================================
 * SourceMod BanId Fix Extension
 * Copyright (C) 2004-2018 AlliedModders LLC.  All rights reserved.
 * =============================================================================
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License, version 3.0, as published by the
 * Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * As a special exception, AlliedModders LLC gives you permission to link the
 * code of this program (as well as its derivative works) to "Half-Life 2," the
 * "Source Engine," the "SourcePawn JIT," and any Game MODs that run on software
 * by the Valve Corporation.  You must obey the GNU General Public License in
 * all respects for all other code used.  Additionally, AlliedModders LLC grants
 * this exception to all derivative works.  AlliedModders LLC defines further
 * exceptions, found in LICENSE.txt (as of this writing, version JULY-31-2007),
 * or <http://www.sourcemod.net/license.php>.
 *
 * Version: $Id$
 */

#include "extension.h"

BanId_Fix g_BanId_Fix;		/**< Global singleton for extension's main interface */
SMEXT_LINK(&g_BanId_Fix);


//ConnectClient
SH_DECL_MANUALHOOK13(ConnectClient, 0, 0, 0, IClient *, const netadr_t &, int, int, int, const char *, const char *, const char *, int, void *, bool, CrossPlayPlatform_t, const unsigned char *, int);
//RejectConnection
SH_DECL_MANUALHOOK1_void_vafmt(RejectConnection, 0, 0, 0, const netadr_t &);
//Filter_IsUserBanned
#ifdef WIN32
bool (__fastcall* Filter_IsUserBanned)(const USERID& userid);
#else
bool (__cdecl* Filter_IsUserBanned)(const USERID& userid);
#endif


IGameConfig *g_pGameConf;
ISDKTools *sdktools = nullptr;
IServer *iserver = nullptr;

ICvar *icvar = nullptr;


ConVar sv_banned_from_server_message("sv_banned_from_server_message", "#Valve_Reject_Banned_From_Server", FCVAR_NONE, "Message that will be displayed to player when they try to access server.");


bool BanId_Fix::SDK_OnLoad(char *error, size_t maxlength, bool late)
{
	char conf_error[255];
	if(!gameconfs->LoadGameConfigFile("BanId_Fix.games", &g_pGameConf, conf_error, sizeof(conf_error)))
	{
		snprintf(error, maxlength, "Could not read BanId_Fix.games: %s", conf_error);
		return false;
	}
	
	int offset;
	if (!g_pGameConf->GetOffset("ConnectClient", &offset) || offset == -1)
	{
		snprintf(error, maxlength, "Failed to get ConnectClient offset.");
		return false;
	}
	SH_MANUALHOOK_RECONFIGURE(ConnectClient, offset, 0, 0);
	
	if (!g_pGameConf->GetOffset("RejectConnection", &offset) || offset == -1)
	{
		snprintf(error, maxlength, "Failed to get RejectConnection offset.");
		return false;
	}
	SH_MANUALHOOK_RECONFIGURE(RejectConnection, offset, 0, 0);
	
	if (!g_pGameConf->GetMemSig("Filter_IsUserBanned", (void**)&Filter_IsUserBanned) || Filter_IsUserBanned == NULL)
	{
		snprintf(error, maxlength, "Failed to get Filter_IsUserBanned signature.");
		return false;
	}
	
	sharesys->AddDependency(myself, "sdktools.ext", true, true);
	
	return true;
}

void BanId_Fix::SDK_OnAllLoaded()
{
	SM_GET_LATE_IFACE(SDKTOOLS, sdktools);
	
	iserver = sdktools->GetIServer();
	
	SH_ADD_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &BanId_Fix::OnClientConnect), false);
}

void BanId_Fix::SDK_OnUnload()
{
	gameconfs->CloseGameConfigFile(g_pGameConf);
	SH_REMOVE_MANUALHOOK(ConnectClient, iserver, SH_MEMBER(this, &BanId_Fix::OnClientConnect), false);
}

bool BanId_Fix::SDK_OnMetamodLoad(ISmmAPI *ismm, char *error, size_t maxlen, bool late)
{
	GET_V_IFACE_CURRENT(GetEngineFactory, icvar, ICvar, CVAR_INTERFACE_VERSION);
	
	g_pCVar = icvar;
	ConVar_Register(0, this);

	return true;
}

bool BanId_Fix::RegisterConCommandBase(ConCommandBase *pCommandBase)
{
	/* Always call META_REGCVAR instead of going through the engine. */
	return META_REGCVAR(pCommandBase);
}

IClient *BanId_Fix::OnClientConnect(const netadr_t & address, int nProtocol, int iChallenge, int nAuthProtocol, const char *pchName, const char *pchPassword, const char *pCookie, int cbCookie, void *pSplitPlayerConnectVector, bool bUnknown, CrossPlayPlatform_t platform, const unsigned char *pUnknown, int iUnknown)
{
	if(nAuthProtocol == 3)
	{
		USERID id;
		
		id.idtype = 1; //IDTYPE_STEAM
		id.uid.steamid.m_SteamInstanceID = 1; //CSGO 1
		id.uid.steamid.m_SteamLocalUserID.Split.High32bits = *(uint32 *)pCookie % 2;
		id.uid.steamid.m_SteamLocalUserID.Split.Low32bits = *(uint32 *)pCookie / 2;
		
		if(Filter_IsUserBanned(id))
		{
			SH_MCALL(iserver, RejectConnection)(address, sv_banned_from_server_message.GetString());
			RETURN_META_VALUE(MRES_SUPERCEDE, nullptr);
		}
	}
	RETURN_META_VALUE(MRES_IGNORED, nullptr);
}