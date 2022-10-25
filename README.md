# BanId Fix

### Description
This plugin is designed to fix an error when banning through banid.
<br>
When a banned player connects to the server, he is kicked, but the server continues to think that the player is just connecting. So there are 'ghosts' that occupy the slot.
<h1 align="center"><img src="https://zizt.fun/uploads/monthly_2018_08/unknown.png.6b368c75a2a745bcd69b3ba3a8b762cb.png"/></h1>

### Dependencies
- [PTaH](https://github.com/komashchenko/PTaH)

### Installation
1. Download [plugin](https://github.com/komashchenko/BanId_Fix/blob/master/BanId_Fix.sp)
2. Compile
3. Upload to server
4. Restart server
5. Enter the command `sm plugins list` in the console and make sure that the plugin is working

### ConVars
The plugin adds the `sv_banned_from_server_message` cvar which allows you to change the message displayed to the kicked player.
<br>
The default value is `#Valve_Reject_Banned_From_Server`
