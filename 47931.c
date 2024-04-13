int lxclock(struct lxc_lock *l, int timeout)
{
	int ret = -1, saved_errno = errno;
	struct flock lk;

	switch(l->type) {
	case LXC_LOCK_ANON_SEM:
		if (!timeout) {
			ret = sem_wait(l->u.sem);
			if (ret == -1)
				saved_errno = errno;
		} else {
			struct timespec ts;
			if (clock_gettime(CLOCK_REALTIME, &ts) == -1) {
				ret = -2;
				goto out;
			}
			ts.tv_sec += timeout;
			ret = sem_timedwait(l->u.sem, &ts);
			if (ret == -1)
				saved_errno = errno;
		}
		break;
	case LXC_LOCK_FLOCK:
		ret = -2;
		if (timeout) {
			ERROR("Error: timeout not supported with flock");
			ret = -2;
			goto out;
		}
		if (!l->u.f.fname) {
			ERROR("Error: filename not set for flock");
			ret = -2;
			goto out;
		}
		if (l->u.f.fd == -1) {
			l->u.f.fd = open(l->u.f.fname, O_RDWR|O_CREAT,
					S_IWUSR | S_IRUSR);
			if (l->u.f.fd == -1) {
				ERROR("Error opening %s", l->u.f.fname);
				goto out;
			}
		}
		lk.l_type = F_WRLCK;
		lk.l_whence = SEEK_SET;
		lk.l_start = 0;
		lk.l_len = 0;
		ret = fcntl(l->u.f.fd, F_SETLKW, &lk);
		if (ret == -1)
			saved_errno = errno;
		break;
	}

out:
	errno = saved_errno;
	return ret;
}
