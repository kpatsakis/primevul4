int lxcunlock(struct lxc_lock *l)
{
	int ret = 0, saved_errno = errno;
	struct flock lk;

	switch(l->type) {
	case LXC_LOCK_ANON_SEM:
		if (!l->u.sem)
			ret = -2;
		else {
			ret = sem_post(l->u.sem);
			saved_errno = errno;
		}
		break;
	case LXC_LOCK_FLOCK:
		if (l->u.f.fd != -1) {
			lk.l_type = F_UNLCK;
			lk.l_whence = SEEK_SET;
			lk.l_start = 0;
			lk.l_len = 0;
			ret = fcntl(l->u.f.fd, F_SETLK, &lk);
			if (ret < 0)
				saved_errno = errno;
			close(l->u.f.fd);
			l->u.f.fd = -1;
		} else
			ret = -2;
		break;
	}

	errno = saved_errno;
	return ret;
}
