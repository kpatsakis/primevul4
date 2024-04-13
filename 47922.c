int container_disk_lock(struct lxc_container *c)
{
	int ret;

	if ((ret = lxclock(c->privlock, 0)))
		return ret;
	if ((ret = lxclock(c->slock, 0))) {
		lxcunlock(c->privlock);
		return ret;
	}
	return 0;
}
