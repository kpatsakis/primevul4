void __rtnl_af_unregister(struct rtnl_af_ops *ops)
{
	list_del(&ops->list);
}
