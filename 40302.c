static rtnl_calcit_func rtnl_get_calcit(int protocol, int msgindex)
{
	struct rtnl_link *tab;

	if (protocol <= RTNL_FAMILY_MAX)
		tab = rtnl_msg_handlers[protocol];
	else
		tab = NULL;

	if (tab == NULL || tab[msgindex].calcit == NULL)
		tab = rtnl_msg_handlers[PF_UNSPEC];

	return tab[msgindex].calcit;
}
