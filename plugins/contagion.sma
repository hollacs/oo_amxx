#include <amxmodx>
#include <fun>
#include <cstrike>
#include <fakemeta>
#include <hamsandwich>
#include <amxmisc>
#include <json>
#include <oo>
#include <contagion_const>

#include "playerclass.pwn"
#include "player.pwn"
#include "manager.pwn"
#include "utils.pwn"

public oo_init()
{
    PlayerClassOO();
    PlayerOO();
    ManagerOO();
}

public plugin_init()
{
    register_plugin("Contagion", "0.1", "colgay");

    ManagerInit();
}

public client_connectex(id)
{
    @call :g_PlayerManager.Connect(id);
}

public client_disconnected(id)
{
    @call :g_PlayerManager.Disconnect(id);
}