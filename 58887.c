int __sys_recvmmsg(int fd, struct mmsghdr __user *mmsg, unsigned int vlen,
		   unsigned int flags, struct timespec *timeout)
{
	int fput_needed, err, datagrams;
	struct socket *sock;
	struct mmsghdr __user *entry;
	struct compat_mmsghdr __user *compat_entry;
	struct msghdr msg_sys;
	struct timespec64 end_time;
	struct timespec64 timeout64;

	if (timeout &&
	    poll_select_set_timeout(&end_time, timeout->tv_sec,
				    timeout->tv_nsec))
		return -EINVAL;

	datagrams = 0;

	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (!sock)
		return err;

	err = sock_error(sock->sk);
	if (err) {
		datagrams = err;
		goto out_put;
	}

	entry = mmsg;
	compat_entry = (struct compat_mmsghdr __user *)mmsg;

	while (datagrams < vlen) {
		/*
		 * No need to ask LSM for more than the first datagram.
		 */
		if (MSG_CMSG_COMPAT & flags) {
			err = ___sys_recvmsg(sock, (struct user_msghdr __user *)compat_entry,
					     &msg_sys, flags & ~MSG_WAITFORONE,
					     datagrams);
			if (err < 0)
				break;
			err = __put_user(err, &compat_entry->msg_len);
			++compat_entry;
		} else {
			err = ___sys_recvmsg(sock,
					     (struct user_msghdr __user *)entry,
					     &msg_sys, flags & ~MSG_WAITFORONE,
					     datagrams);
			if (err < 0)
				break;
			err = put_user(err, &entry->msg_len);
			++entry;
		}

		if (err)
			break;
		++datagrams;

		/* MSG_WAITFORONE turns on MSG_DONTWAIT after one packet */
		if (flags & MSG_WAITFORONE)
			flags |= MSG_DONTWAIT;

		if (timeout) {
			ktime_get_ts64(&timeout64);
			*timeout = timespec64_to_timespec(
					timespec64_sub(end_time, timeout64));
			if (timeout->tv_sec < 0) {
				timeout->tv_sec = timeout->tv_nsec = 0;
				break;
			}

			/* Timeout, return less than vlen datagrams */
			if (timeout->tv_nsec == 0 && timeout->tv_sec == 0)
				break;
		}

		/* Out of band data, return right away */
		if (msg_sys.msg_flags & MSG_OOB)
			break;
		cond_resched();
	}

	if (err == 0)
		goto out_put;

	if (datagrams == 0) {
		datagrams = err;
		goto out_put;
	}

	/*
	 * We may return less entries than requested (vlen) if the
	 * sock is non block and there aren't enough datagrams...
	 */
	if (err != -EAGAIN) {
		/*
		 * ... or  if recvmsg returns an error after we
		 * received some datagrams, where we record the
		 * error to return on the next call or if the
		 * app asks about it using getsockopt(SO_ERROR).
		 */
		sock->sk->sk_err = -err;
	}
out_put:
	fput_light(sock->file, fput_needed);

	return datagrams;
}
