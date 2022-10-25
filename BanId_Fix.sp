#pragma semicolon 1
#pragma newdecls required
#include <sdktools>
#include <PTaH>

public Plugin myinfo =
{
	name = "BanId Fix",
	author = "Phoenix (˙·٠●Феникс●٠·˙)",
	version = "1.0.1",
	url = "https://github.com/komashchenko/BanId_Fix"
};

bool g_bWindows;
ConVar sv_banned_from_server_message;
Handle g_hFilter_IsUserBanned;
Handle g_hFilter_Steam64bitIdToUserID;

public void OnPluginStart()
{
	GameData hGameData = LoadGameConfigFile("BanId_Fix.games");
	
	if (!hGameData)
	{
		SetFailState("Couldn't load BanId_Fix.games game config.");
		
		return;
	}
	
	// Is windows
	char szBuf[14];
	GetCommandLine(szBuf, sizeof szBuf);
	g_bWindows = strcmp(szBuf, "./srcds_linux") != 0;
	
	// Filter_IsUserBanned
	if(g_bWindows)
	{
		// Actually there fastcall
		StartPrepSDKCall(SDKCall_Raw);
	}
	else
	{
		StartPrepSDKCall(SDKCall_Static);
		// We need to pass a pointer to an object (:
		PrepSDKCall_AddParameter(SDKType_String, SDKPass_Pointer);
	}
	PrepSDKCall_SetReturnInfo(SDKType_Bool, SDKPass_Plain);
	PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "Filter_IsUserBanned");
	
	if(!(g_hFilter_IsUserBanned = EndPrepSDKCall()))
	{
		SetFailState("Failed to create call for Filter_IsUserBanned");
		
		return;
	}
	
	if(g_bWindows)
	{
		// Filter_Steam64bitIdToUserID
		StartPrepSDKCall(SDKCall_Static);
		PrepSDKCall_SetFromConf(hGameData, SDKConf_Signature, "Filter_Steam64bitIdToUserID");
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_AddParameter(SDKType_PlainOldData, SDKPass_Plain);
		PrepSDKCall_SetReturnInfo(SDKType_PlainOldData, SDKPass_Plain);
		
		if(!(g_hFilter_Steam64bitIdToUserID = EndPrepSDKCall()))
		{
			SetFailState("Failed to create call for Filter_Steam64bitIdToUserID");
			
			return;
		}
	}
	
	hGameData.Close();
	
	sv_banned_from_server_message = CreateConVar("sv_banned_from_server_message", "#Valve_Reject_Banned_From_Server", "Message that will be displayed to player when they try to access server.");
	
	PTaH(PTaH_ClientConnectPre, Hook, ClientConnectPre);
}

Action ClientConnectPre(int iAccountID, const char[] sIP, const char[] sName, char sPassword[128], char sRejectReason[255])
{
	bool bUserBanned;
	
	if(g_bWindows)
	{
		bUserBanned = SDKCall(g_hFilter_IsUserBanned, SDKCall(g_hFilter_Steam64bitIdToUserID, iAccountID, 17825793));
	}
	else
	{
		// USERID_t
		int id[4];
		id[0] = 1; // id.idtype = IDTYPE_STEAM;
		id[1] = 1; // id.uid.steamid.m_SteamInstanceID = 1;
		id[2] = iAccountID >>> 1;// id.uid.steamid.m_SteamLocalUserID.Split.Low32bits
		id[3] = iAccountID & 1;// id.uid.steamid.m_SteamLocalUserID.Split.High32bits
		
		bUserBanned = SDKCall(g_hFilter_IsUserBanned, id);
	}
	
	if(bUserBanned)
	{
		sv_banned_from_server_message.GetString(sRejectReason, sizeof sRejectReason);
		
		return Plugin_Handled;
	}
	
	return Plugin_Continue;
}