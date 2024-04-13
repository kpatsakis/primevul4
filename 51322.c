void sock_unregister(int family)
{
	BUG_ON(family < 0 || family >= NPROTO);

	spin_lock(&net_family_lock);
	RCU_INIT_POINTER(net_families[family], NULL);
	spin_unlock(&net_family_lock);

	synchronize_rcu();

	pr_info("NET: Unregistered protocol family %d\n", family);
}
