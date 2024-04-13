void container_mem_unlock(struct lxc_container *c)
{
	lxcunlock(c->privlock);
}
