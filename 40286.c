void rtnl_af_unregister(struct rtnl_af_ops *ops)
{
	rtnl_lock();
	__rtnl_af_unregister(ops);
	rtnl_unlock();
}
