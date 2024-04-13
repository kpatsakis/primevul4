static int __init nfnetlink_init(void)
{
	int i;

	for (i = NFNLGRP_NONE + 1; i <= NFNLGRP_MAX; i++)
		BUG_ON(nfnl_group2type[i] == NFNL_SUBSYS_NONE);

	for (i=0; i<NFNL_SUBSYS_COUNT; i++)
		mutex_init(&table[i].mutex);

	pr_info("Netfilter messages via NETLINK v%s.\n", nfversion);
	return register_pernet_subsys(&nfnetlink_net_ops);
}
