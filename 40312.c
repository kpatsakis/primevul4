void rtnl_link_unregister(struct rtnl_link_ops *ops)
{
	rtnl_lock();
	__rtnl_link_unregister(ops);
	rtnl_unlock();
}
