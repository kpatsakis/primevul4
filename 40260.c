int __rtnl_af_register(struct rtnl_af_ops *ops)
{
	list_add_tail(&ops->list, &rtnl_af_ops);
	return 0;
}
