asmlinkage long sys_oabi_sendmsg(int fd, struct user_msghdr __user *msg, unsigned flags)
{
	struct sockaddr __user *addr;
	int msg_namelen;
	sa_family_t sa_family;
	if (msg &&
	    get_user(msg_namelen, &msg->msg_namelen) == 0 &&
	    msg_namelen == 112 &&
	    get_user(addr, &msg->msg_name) == 0 &&
	    get_user(sa_family, &addr->sa_family) == 0 &&
	    sa_family == AF_UNIX)
	{
		/*
		 * HACK ALERT: there is a limit to how much backward bending
		 * we should do for what is actually a transitional
		 * compatibility layer.  This already has known flaws with
		 * a few ioctls that we don't intend to fix.  Therefore
		 * consider this blatent hack as another one... and take care
		 * to run for cover.  In most cases it will "just work fine".
		 * If it doesn't, well, tough.
		 */
		put_user(110, &msg->msg_namelen);
	}
	return sys_sendmsg(fd, msg, flags);
}
