void container_disk_unlock(struct lxc_container *c)
{
	lxcunlock(c->slock);
	lxcunlock(c->privlock);
}
