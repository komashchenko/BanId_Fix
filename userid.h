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

#pragma once

//
// Old Steam2 ID stuff
typedef	unsigned short		SteamInstanceID_t;		// MUST be 16 bits
typedef uint64			SteamLocalUserID_t;		// MUST be 64 bits

// Old steam2 user ID structures
typedef struct	
{
	unsigned int	Low32bits;
	unsigned int	High32bits;
}	TSteamSplitLocalUserID;

typedef struct
{
	SteamInstanceID_t		m_SteamInstanceID;

	union
	{
		SteamLocalUserID_t		As64bits;
		TSteamSplitLocalUserID	Split;
	}						m_SteamLocalUserID;

} TSteamGlobalUserID;

//https://github.com/VSES/SourceEngine2007/blob/43a5c90a5ada1e69ca044595383be67f40b33c61/src_main/common/userid.h
struct USERID //USERID_t hl2sdk-csgo/public/iclient.h:20:8: note: forward declaration of 'USERID_t'
{
	int			idtype;

	union
	{
		TSteamGlobalUserID	steamid;
	} uid;
};