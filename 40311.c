int rtnl_link_register(struct rtnl_link_ops *ops)
{
	int err;

	rtnl_lock();
	err = __rtnl_link_register(ops);
	rtnl_unlock();
	return err;
}
