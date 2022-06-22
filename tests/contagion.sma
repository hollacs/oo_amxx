#include <amxmodx>
#include <oo>

public oo_init()
{
	class<"PlayerClassInfo">@
	{
		new const c[] = "PlayerClassInfo";

		ivar<c>@Public["m_Name"]:OO_ARRAY[32]@
		ivar<c>@Public["m_Desc"]:OO_ARRAY[32]@
		ivar<c>@Public["m_Team"]:OO_CELL@
		ivar<c>@Public["m_Flags"]:OO_CELL@
		ivar<c>@Public["m_PlayerModels"]:OO_CELL@ // Array:
		ivar<c>@Public["m_ViewModels"]:OO_CELL@ // Trie:
		ivar<c>@Public["m_WeaponModels"]:OO_CELL@ // Trie:
		ivar<c>@Public["m_Sounds"]:OO_CELL@ // Trie:
		ivar<c>@Public["m_Cvars"]:OO_ARRAY[4]@

		// (const name[], const desc[], team, flags)
		ctor<c>@Public."Ctor"(OO_ARRAY[32], OO_ARRAY[32], OO_CELL, OO_CELL)@
		dtor<c>@Public."Dtor"()@

		// (hp, Float:gravity, Float:speed, Float:knockback)
		//msg<c>@Public."CreateCvars"(OO_CELL, OO_CELL, OO_CELL, OO_CELL)@

		//void_msg<c>@Public."LoadAssets"()@
		//msg<c>@Public."LoadJson"(OO_STRING)@ // (const filename[])
	}

	class<"PlayerClass">@
	{
		new const c[] = "PlayerClass";

		ivar<c>@Public["m_oPlayer"]:OO_CELL@ // Obj:
		ivar<c>@Public["m_oPlayerClassInfo"]:OO_CELL@ // Obj:

		ctor<c>@Public."Ctor"(OO_CELL)@ // (Obj:oPlayer)
		dtor<c>@Public."Dtor"()@

		msg<c>@Public."AssignInfo"(OO_CELL)@ // (Obj:oInfo)
	}

	class<"Player">@
	{
		new const c[] = "Player";

		ivar<c>@Public["m_PlayerId"]:OO_CELL@
		ivar<c>@Public["m_oPlayerClass"]:OO_CELL@ // Obj:

		ctor<c>@Public."Ctor"(OO_CELL)@ // (id)
		dtor<c>@Public."Dtor"()@

		void_msg<c>@Public."Connect"()@
		void_msg<c>@Public."Disconnect"()@
	}

	class<"PlayerHandler">@
	{
		new const c[] = "PlayerHandler";

		ivar<c>@Public["m_oPlayers"]:OO_ARRAY[MAX_PLAYERS+1]@

		void_ctor<c>@Public."Ctor"()@
		void_dtor<c>@Public."Dtor"()@

		msg<c>@Public."Connect"(OO_CELL)@ // (id)
		msg<c>@Public."Disconnect"(OO_CELL)@ // (id)
		msg<c>@Public."GetPlayerObj"(OO_CELL)@ // (id)
	}
}

new Obj:g_oPlayerHandler;

public plugin_init()
{
	register_plugin("Contagion", "0.1", "colg");

	g_oPlayerHandler = alloc<"PlayerHandler">();
}

/* ---------- [PlayerClassInfo] ---------- */

public PlayerClassInfo@Ctor(const name[], const desc[], team, flags)
{
	new Obj:this = oo_this();

	write_a#this["m_Name"][0..32] << { name[0..strlen(name)] }
	write_a#this["m_Desc"][0..32] << { desc[0..strlen(desc)] }
	write#this["m_Team"] << { team }
	write#this["m_Flags"] << { flags }

	write#this["m_PlayerModels"] << { ArrayCreate(1) }
	write#this["m_ViewModels"] << { TrieCreate() }
	write#this["m_WeaponModels"] << { TrieCreate() }
	write#this["m_Sounds"] << { TrieCreate() }
}

public PlayerClassInfo@Dtor()
{
	new Obj:this = oo_this();

	new Array:playermodels = Array:read#this["m_PlayerModels"];
	new Trie:viewmodels = Trie:read#this["m_ViewModels"];
	new Trie:weaponmodels = Trie:read#this["m_WeaponModels"];
	new Trie:sounds = Trie:read#this["m_Sounds"];

	ArrayDestroy(playermodels);
	TrieDestroy(viewmodels);
	TrieDestroy(weaponmodels);
	TrieDestroy(sounds);
}

/* ---------- [PlayerClass] ---------- */

public PlayerClass@Ctor(Obj:oPlayer)
{
	write#oo_this()["m_oPlayer"] << { oPlayer }
	write#oo_this()["m_oPlayerClassInfo"] << { @null }
}

public PlayerClass@Dtor(Obj:oPlayer)
{
}

public PlayerClass@AssignInfo(Object:oInfo)
{
	write#oo_this()["m_oPlayerClassInfo"] << { oInfo }
}

/* ---------- [Player] ---------- */

public Player@Ctor(id)
{
	write#oo_this()["m_PlayerId"] << { id }
	write#oo_this()["m_oPlayerClass"] << { @null }
	server_print("Player@Ctor()");
}

public Player@Dtor()
{
	server_print("Player@Dtor()");
}

public Player@Connect()
{
	new player_id = read#oo_this()["m_PlayerId"];
	server_print("Player@Connect(%d)", player_id);
}

public Player@Disconnect()
{
	new player_id = read#oo_this()["m_PlayerId"];
	server_print("Player@Disconnect(%d)", player_id);
}

/* ---------- [PlayerHandler] ---------- */

public PlayerHandler@Ctor()
{
	new Obj:oPlayers[MAX_PLAYERS+1] = {Obj:@null, ...};
	write_a#oo_this()["m_oPlayers"][1..MaxClients] << { oPlayers[1..MaxClients] }
}

public PlayerHandler@Dtor()
{
	new Obj:oPlayers[MAX_PLAYERS+1] = {Obj:@null, ...};
	read_a#oo_this()["m_oPlayers"][1..MaxClients] >> { oPlayers[1..MaxClients] }

	for (new i = 1; i <= MaxClients; i++)
	{
		if (oPlayers[i] != Obj:@null)
		{
			delete@(oPlayers[i]);
		}
	}
}

public PlayerHandler@Connect(id)
{
	new Obj:this = oo_this();

	new Obj:oPlayers[MAX_PLAYERS+1] = {Obj:@null, ...};
	read_a#this["m_oPlayers"][1..MaxClients] >> { oPlayers[1..MaxClients] }

	if (oPlayers[id] == Obj:@null)
	{
		oPlayers[id] = new<"Player">(id);
		write_a#this["m_oPlayers"][1..MaxClients] << { oPlayers[1..MaxClients] }

		void_send#oPlayers[id]."Connect"();
	}
}

public PlayerHandler@Disconnect(id)
{
	new Obj:this = oo_this();

	new Obj:oPlayers[MAX_PLAYERS+1] = {Obj:@null, ...};
	read_a#this["m_oPlayers"][1..MaxClients] >> { oPlayers[1..MaxClients] }

	if (oPlayers[id] != Obj:@null)
	{
		void_send#oPlayers[id]."Disconnect"();

		delete@(oPlayers[id]);
		oPlayers[id] = Obj:@null;
		write_a#this["m_oPlayers"][1..MaxClients] << { oPlayers[1..MaxClients] }
	}
}

public Obj:PlayerHandler@GetPlayer(id)
{
	new Obj:oPlayer[1];
	read_a#oo_this()["m_oPlayers"][id..(id+1)] >> { oPlayer[0..1] }

	return oPlayer[0];
}

public client_connectex(id)
{
	send#g_oPlayerHandler."Connect"(id);
}

public client_disconnected(id)
{
	send#g_oPlayerHandler."Disconnect"(id);
}