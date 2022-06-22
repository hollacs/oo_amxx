#if defined _ctg_playerclass_included
	#endinput
#endif

#define _ctg_playerclass_included

PlayerClassOO()
{
	@class ("PlayerClassInfo")
	{
		@init_class("PlayerClassInfo");

		@var (OO_ARRAY[32]:m_Name);
		@var (OO_ARRAY[32]:m_Desc);
		@var (OO_CELL:m_Team);
		@var (OO_CELL:m_Flags);
		@var (OO_CELL:m_Models); // Array:
		@var (OO_CELL:m_vModels); // Trie:
		@var (OO_CELL:m_pModels); // Trie:
		@var (OO_CELL:m_Sounds); // Trie:
		@var (OO_CELL:m_Cvars) // Trie:

		// (const name[], const desc[], team, flags)
		@construct :Ctor(@string, @string, @cell, @cell);
		@destruct  :Dtor();

		// (const name[], const string[], flags, const description[])
		@method :CreateCvar(@string, @string, @cell, @string);

		@method0 :LoadAssets();
		@method  :LoadJson(@string); // (const filename[])
	}

	@class ("PlayerClass")
	{
		@init_class("PlayerClass");

		@var (OO_CELL:m_oPlayer); // Obj:
		@var (OO_CELL:m_oClassInfo); // Obj:

		@construct :Ctor(@cell); // (Obj:player_obj)
		@destruct  :Dtor();

		@method0 :GetPlayerIndex();
		@method0 :AssignClassInfo();
		@method0 :SetProperties();
		@method0 :SetMaxSpeed();
		@method  :SetWeaponModel(@cell); // (entity)

		// (channel, const sample[], Float:volume, Float:attn, flags, pitch)
		@method :ChangeSound(@cell, @string, @float, @float, @cell, @cell);
	}
}

public PlayerClassInfo@Ctor(const name[], const desc[], team, flags)
{
	@init_this(this);

	@sets (this.m_Name[] << name);
	@sets (this.m_Desc[] << desc);
	@sets (this.m_Team[] << team);
	@set  (this.m_Flags: = flags);

	@set (this.m_Models: 	= ArrayCreate(32));
	@set (this.m_vModels: 	= TrieCreate());
	@set (this.m_pModels: 	= TrieCreate());
	@set (this.m_Sounds: 	= TrieCreate());
	@set (this.m_Cvars: 	= TrieCreate());

	@call0 :this.LoadAssets();
}

public PlayerClassInfo@Dtor() {}

public PlayerClassInfo@CreateCvar(const name[], const string[], flags, const description[])
{
	@init_this(this);

	static cvar_name[50], class_name[32];
	@gets (this.m_Name[] >> class_name[32]);
	strtolower(class_name);

	formatex(cvar_name, charsmax(cvar_name), "ctg_%s_%s", class_name, name);
	TrieSetCell(Trie:@get(this.m_Cvars), name, create_cvar(cvar_name, string, flags, description));
}

public PlayerClassInfo@LoadAssets() // override this
{
	@call: _this.LoadJson("player");
}

public PlayerClassInfo@LoadJson(const filename[])
{
	@init_this(this);

	static path[100];
	get_configsdir(path, charsmax(path));
	format(path, charsmax(path), "%s/contagion/playerclass/%s.json", path, filename);

	new JSON:json = json_parse(path, true, true);
	if (json != Invalid_JSON)
	{
		static key[128], value[128];

		// load player models
		new JSON:models = json_object_get_value(json, "models");
		if (models != Invalid_JSON)
		{
			new Array:aModels = Array:@get(this.m_Models);
			for (new i = json_array_get_count(models) - 1; i >= 0; i--)
			{
				json_array_get_string(models, i, value, charsmax(value));
				ArrayPushString(aModels, value);
				PrecachePlayerModel(value);
			}
			json_free(models);
		}

		// load view models
		new JSON:vmodels_json = json_object_get_value(json, "vmodels_json");
		if (vmodels_json != Invalid_JSON)
		{
			new JSON:vmodel_val = Invalid_JSON;
			new Trie:vmodel_trie = Trie:@get(this.m_vModels);
			for (new i = json_object_get_count(vmodels_json) - 1; i >= 0; i--)
			{
				json_object_get_name(vmodels_json, i, key, charsmax(key));
				vmodel_val = json_object_get_value_at(vmodels_json, i);
				json_get_string(vmodel_val, value, charsmax(value));
				json_free(vmodel_val);
				TrieSetString(vmodel_trie, key, value);
				if (value[0])
					precache_model(value);
			}
			json_free(vmodels_json);
		}

		// load 3rd person models
		new JSON:pmodel_json = json_object_get_value(json, "pmodel_json");
		if (pmodel_json != Invalid_JSON)
		{
			new JSON:pmodel_val = Invalid_JSON;
			new Trie:pmodel_trie = Trie:@get(this.m_pModels);
			for (new i = json_object_get_count(pmodel_json) - 1; i >= 0; i--)
			{
				json_object_get_name(pmodel_json, i, key, charsmax(key));
				pmodel_val = json_object_get_value_at(pmodel_json, i);
				json_get_string(pmodel_val, value, charsmax(value));
				json_free(pmodel_val);
				TrieSetString(pmodel_trie, key, value);
				if (value[0])
					precache_model(value);
			}
			json_free(pmodel_json);
		}

		// load sound_json
		new JSON:sound_json = json_object_get_value(json, "sound_json");
		if (sound_json != Invalid_JSON)
		{
			new Array:sound_arr = Invalid_Array;
			new JSON:sound_val = Invalid_JSON;
			new Trie:sound_trie = Trie:@get(this.m_Sounds);
			for (new i = json_object_get_count(sound_json) - 1; i >= 0; i--)
			{
				json_object_get_name(sound_json, i, key, charsmax(key));
				sound_val = json_object_get_value_at(sound_json, i);
				sound_arr = ArrayCreate(100);
				for (new i = json_array_get_count(sound_val) - 1; i >= 0; i--)
				{
					json_array_get_string(sound_val, i, value, charsmax(value));
					ArrayPushString(sound_arr, value);
					if (value[0])
						precache_sound(value);
				}
				json_free(sound_val);
				TrieSetCell(sound_trie, key, sound_arr);
			}
			json_free(sound_json);
		}

		json_free(json);
	}
}

// constructor
public PlayerClass@Ctor(Object:player_obj)
{
	@init_this(this);
	@set (this.m_oPlayer: = player_obj);
	@call0 :this.AssignClassInfo();
}

public PlayerClass@Dtor() {}

// Get player index
public PlayerClass@GetPlayerIndex()
{
	new Player:player_obj = any:@get(_this.m_oPlayer);
	if (player_obj == @null)
		return 0;
	
	return @get(player_obj.m_PlayerId);
}

// Assign player class info
public PlayerClass@AssignClassInfo() // override this?
{
	@set (_this.m_oClassInfo: = @null); // assign nothing
}

public PlayerClass@SetProperties()
{
	@init_this(this);

	new PlayerClassInfo:info_obj = any:@get(this.m_oClassInfo);
	if (info_obj == @null)
		return;

	new id = @call0:this.GetPlayerIndex();
	new Trie:tcvars = Trie:@get(info_obj.m_Cvars);
	new pcvar;

	// has health
	if (TrieGetCell(tcvars, "hp", pcvar))
		set_user_health(id, get_pcvar_num(pcvar));

	// has gravity
	if (TrieGetCell(tcvars, "gravity", pcvar))
		set_user_gravity(id, get_pcvar_float(pcvar));

	new Array:model_arr = Array:@get(info_obj.m_Models);
	new model_size = ArraySize(model_arr);
	if (model_size > 0) // has model
	{
		static buffer[32];
		ArrayGetString(model_arr, random(model_size), buffer, charsmax(buffer));
		cs_set_user_model(id, buffer);
	}

	ExecuteHamB(Ham_CS_Player_ResetMaxSpeed, id); // update maxspeed

	new ent = get_ent_data_entity(id, "CBasePlayer", "m_pActiveItem");
	if (pev_valid(ent)) ExecuteHamB(Ham_Item_Deploy, ent); // update weapon model
}

public PlayerClass@SetMaxSpeed()
{
	@init_this(this);

	new PlayerClassInfo:info_obj = any:@get(this.m_oClassInfo);
	if (info_obj == @null)
		return;

	new id = @call0:this.GetPlayerIndex();
	new Trie:tcvars = Trie:@get(info_obj.m_Cvars);
	new pcvar;

	if (TrieGetCell(tcvars, "speed", pcvar)) // has maxspeed
	{
		new Float:speed = get_pcvar_float(pcvar);
		set_user_maxspeed(id, (speed < 5.0) ? get_user_maxspeed(id) * speed : speed);
	}
}

public PlayerClass@SetWeaponModel(ent)
{
	@init_this(this);

	new PlayerClassInfo:info_obj = any:@get(this.m_oClassInfo);
	if (info_obj == @null)
		return;

	static classname[32], model[100];
	pev(ent, pev_classname, classname, charsmax(classname));

	new id = @call0:this.GetPlayerIndex();

	// has v_model
	if (TrieGetString(Trie:@get(info_obj.m_vModels), classname, model, charsmax(model)))
	{
		set_pev(id, pev_viewmodel2, model);
	}

	// has p_model
	if (TrieGetString(Trie:@get(info_obj.m_pModels), classname, model, charsmax(model)))
	{
		set_pev(id, pev_weaponmodel2, model);
	}
}

// return true for blocking the original EmitSound() forward
public bool:PlayerClass@ChangeSound(channel, const sample[], &Float:volume, Float:attenuation, flags, pitch)
{
	@init_this(this);

	new PlayerClassInfo:info_obj = any:@get(this.m_oClassInfo);
	if (info_obj == @null)
		return false;

	static sound[100], Array:sound_arr;

	// has sound
	if (TrieGetCell(Trie:@get(info_obj.m_Sounds), sample, sound_arr))
	{
		new id = @call0:this.GetPlayerIndex();
		ArrayGetString(sound_arr, random(ArraySize(sound_arr)), sound, charsmax(sound));
		emit_sound(id, channel, sound, volume, attenuation, flags, pitch);
		return true;
	}

	return false;
}