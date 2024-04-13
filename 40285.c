int rtnl_af_register(struct rtnl_af_ops *ops)
{
	int err;

	rtnl_lock();
	err = __rtnl_af_register(ops);
	rtnl_unlock();
	return err;
}
