#if defined _ctg_manager_included
	#endinput
#endif

#define _ctg_manager_included

new PlayerManager:g_PlayerManager;

ManagerOO()
{
    @class ("PlayerManager")
    {
        @init_class("PlayerManager");

        @var (OO_ARRAY[MAX_PLAYERS+1]:m_oPlayers);

        @construct0 :Ctor();

        @method :PlayerOfIndex(@cell); // Player:(id)
        @method :IndexOfPlayer(@cell); // (Player:obj)
        
        @method :Connect(@cell); // (id)
        @method :Disconnect(@cell); // (id)
    }
}

ManagerInit()
{
    g_PlayerManager = any:@new0("PlayerManager");
}

public PlayerManager@Ctor()
{
    /* not needed?
    new Player:objs[MAX_PLAYERS+1] = {@null, ...};
    @seta(_this.m_oPlayer[0..sizeof(objs)] >> objs[0..sizeof(objs)]);
    */
}

public Player:PlayerManager@PlayerOfIndex(id)
{
    if ((1 <= id <= MaxClients))
    {
        new Player:obj = @null;
        @geta(_this.m_oPlayers[id..id+1] >> obj[0..1]);
        return obj;
    }

    return @null;
}

public PlayerManager@IndexOfPlayer(Player:obj)
{
    return @get(obj.m_PlayerIndex);
}

public PlayerManager@Connect(id)
{
    new Player:obj = any:@new("Player", id);
    @seta(_this.m_oPlayers[id..id+1] << obj[0..1]);
}

public PlayerManager@Disconnect(id)
{
    new Player:obj = @null;
    @geta(_this.m_oPlayers[id..id+1] >> obj[0..1]);
    if (obj != @null)
    {
        @delete (obj);
        obj = @null;
        @seta (_this.m_oPlayers[id..id+1] << obj[0..1]);
    }
}