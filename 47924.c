int container_mem_lock(struct lxc_container *c)
{
	return lxclock(c->privlock, 0);
}
