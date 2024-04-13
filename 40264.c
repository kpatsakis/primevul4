int __rtnl_register(int protocol, int msgtype,
		    rtnl_doit_func doit, rtnl_dumpit_func dumpit,
		    rtnl_calcit_func calcit)
{
	struct rtnl_link *tab;
	int msgindex;

	BUG_ON(protocol < 0 || protocol > RTNL_FAMILY_MAX);
	msgindex = rtm_msgindex(msgtype);

	tab = rtnl_msg_handlers[protocol];
	if (tab == NULL) {
		tab = kcalloc(RTM_NR_MSGTYPES, sizeof(*tab), GFP_KERNEL);
		if (tab == NULL)
			return -ENOBUFS;

		rtnl_msg_handlers[protocol] = tab;
	}

	if (doit)
		tab[msgindex].doit = doit;

	if (dumpit)
		tab[msgindex].dumpit = dumpit;

	if (calcit)
		tab[msgindex].calcit = calcit;

	return 0;
}
