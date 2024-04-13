static void rollback_registered(struct net_device *dev)
{
	LIST_HEAD(single);

	list_add(&dev->unreg_list, &single);
	rollback_registered_many(&single);
	list_del(&single);
}
