void scm_detach_fds_compat(struct msghdr *kmsg, struct scm_cookie *scm)
{
	struct compat_cmsghdr __user *cm = (struct compat_cmsghdr __user *) kmsg->msg_control;
	int fdmax = (kmsg->msg_controllen - sizeof(struct compat_cmsghdr)) / sizeof(int);
	int fdnum = scm->fp->count;
	struct file **fp = scm->fp->fp;
	int __user *cmfptr;
	int err = 0, i;

	if (fdnum < fdmax)
		fdmax = fdnum;

	for (i = 0, cmfptr = (int __user *) CMSG_COMPAT_DATA(cm); i < fdmax; i++, cmfptr++) {
		int new_fd;
		err = security_file_receive(fp[i]);
		if (err)
			break;
		err = get_unused_fd_flags(MSG_CMSG_CLOEXEC & kmsg->msg_flags
					  ? O_CLOEXEC : 0);
		if (err < 0)
			break;
		new_fd = err;
		err = put_user(new_fd, cmfptr);
		if (err) {
			put_unused_fd(new_fd);
			break;
		}
		/* Bump the usage count and install the file. */
		fd_install(new_fd, get_file(fp[i]));
	}

	if (i > 0) {
		int cmlen = CMSG_COMPAT_LEN(i * sizeof(int));
		err = put_user(SOL_SOCKET, &cm->cmsg_level);
		if (!err)
			err = put_user(SCM_RIGHTS, &cm->cmsg_type);
		if (!err)
			err = put_user(cmlen, &cm->cmsg_len);
		if (!err) {
			cmlen = CMSG_COMPAT_SPACE(i * sizeof(int));
			kmsg->msg_control += cmlen;
			kmsg->msg_controllen -= cmlen;
		}
	}
	if (i < fdnum)
		kmsg->msg_flags |= MSG_CTRUNC;

	/*
	 * All of the files that fit in the message have had their
	 * usage counts incremented, so we just free the list.
	 */
	__scm_destroy(scm);
}
