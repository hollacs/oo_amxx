#if defined _ctg_player_included
	#endinput
#endif

#define _ctg_player_included

PlayerOO()
{
	@class ("Player")
	{
		@init_class("Player");

		@var (OO_CELL:m_PlayerIndex);
		@var (OO_CELL:m_Team); // PlayerTeam
		@var (OO_CELL:m_oClass); // Obj

		@construct :Ctor(@cell); // (player_id)
		@destruct :Dtor();

		@method0 :Connect();
		@method0 :Disconnect();
		@method  :ChangeClass(@string); // (const classname[])
	}
}

public Player@Ctor(player_id)
{
	@init_this(this);
	@set (this.m_PlayerIndex:  = player_id);
	@set (this.m_Team:   = Team_None);
	@set (this.m_oClass: = @null);

	server_print("Player@Ctor()");
}

public Player@Dtor()
{
	server_print("Player@Dtor()");
}

// Change player class
public Player@ChangeClass(const dest_class[])
{
	@init_this(this);

	new PlayerClass:class_obj = any:@get(this.m_oClass);
	if (class_obj != @null)
	{
		// delete old player class
		@delete (class_obj);
		@set (this.m_oClass: = @null);
	}

	// safe check: if dest_class is vaild
	if (oo_class_exists(dest_class) && oo_subclass_of(dest_class, "PlayerClass"))
	{
		class_obj = @new (dest_class, this); // new object
		@set (this.m_oClass: = class_obj); // assign to the player

		new PlayerClassInfo:o_info = any:@get(class_obj.m_oClassInfo);
		@set (this.m_Team: = @get(o_info.m_Team)); // set player team

		@call0 :class_obj.SetProperties(); // set player properties
	}
}

// unused
public Player@Connect() {}
public Player@Disconnect() {}