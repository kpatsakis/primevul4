int LuaSettings::gc_object(lua_State* L)
{
	LuaSettings* o = *(LuaSettings **)(lua_touserdata(L, 1));
	delete o;
	return 0;
}