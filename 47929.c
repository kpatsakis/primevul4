struct lxc_lock *lxc_newlock(const char *lxcpath, const char *name)
{
	struct lxc_lock *l;

	l = malloc(sizeof(*l));
	if (!l)
		goto out;

	if (!name) {
		l->type = LXC_LOCK_ANON_SEM;
		l->u.sem = lxc_new_unnamed_sem();
		if (!l->u.sem) {
			free(l);
			l = NULL;
		}
		goto out;
	}

	l->type = LXC_LOCK_FLOCK;
	l->u.f.fname = lxclock_name(lxcpath, name);
	if (!l->u.f.fname) {
		free(l);
		l = NULL;
		goto out;
	}
	l->u.f.fd = -1;

out:
	return l;
}
