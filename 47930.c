void lxc_putlock(struct lxc_lock *l)
{
	if (!l)
		return;
	switch(l->type) {
	case LXC_LOCK_ANON_SEM:
		if (l->u.sem) {
			sem_destroy(l->u.sem);
			free(l->u.sem);
			l->u.sem = NULL;
		}
		break;
	case LXC_LOCK_FLOCK:
		if (l->u.f.fd != -1) {
			close(l->u.f.fd);
			l->u.f.fd = -1;
		}
		free(l->u.f.fname);
		l->u.f.fname = NULL;
		break;
	}
	free(l);
}
