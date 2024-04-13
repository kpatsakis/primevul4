int __sys_sendmmsg(int fd, struct mmsghdr __user *mmsg, unsigned int vlen,
		   unsigned int flags)
{
	int fput_needed, err, datagrams;
	struct socket *sock;
	struct mmsghdr __user *entry;
	struct compat_mmsghdr __user *compat_entry;
	struct msghdr msg_sys;
	struct used_address used_address;
	unsigned int oflags = flags;

	if (vlen > UIO_MAXIOV)
		vlen = UIO_MAXIOV;

	datagrams = 0;

	sock = sockfd_lookup_light(fd, &err, &fput_needed);
	if (!sock)
		return err;

	used_address.name_len = UINT_MAX;
	entry = mmsg;
	compat_entry = (struct compat_mmsghdr __user *)mmsg;
	err = 0;
	flags |= MSG_BATCH;

	while (datagrams < vlen) {
		if (datagrams == vlen - 1)
			flags = oflags;

		if (MSG_CMSG_COMPAT & flags) {
			err = ___sys_sendmsg(sock, (struct user_msghdr __user *)compat_entry,
					     &msg_sys, flags, &used_address, MSG_EOR);
			if (err < 0)
				break;
			err = __put_user(err, &compat_entry->msg_len);
			++compat_entry;
		} else {
			err = ___sys_sendmsg(sock,
					     (struct user_msghdr __user *)entry,
					     &msg_sys, flags, &used_address, MSG_EOR);
			if (err < 0)
				break;
			err = put_user(err, &entry->msg_len);
			++entry;
		}

		if (err)
			break;
		++datagrams;
		cond_resched();
	}

	fput_light(sock->file, fput_needed);

	/* We only return an error if no datagrams were able to be sent */
	if (datagrams != 0)
		return datagrams;

	return err;
}
