is_server (server *serv)
{
	return g_slist_find (serv_list, serv) ? 1 : 0;
}
