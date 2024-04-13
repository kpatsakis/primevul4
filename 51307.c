static int copy_msghdr_from_user(struct msghdr *kmsg,
				 struct user_msghdr __user *umsg,
				 struct sockaddr __user **save_addr,
				 struct iovec **iov)
{
	struct sockaddr __user *uaddr;
	struct iovec __user *uiov;
	size_t nr_segs;
	ssize_t err;

	if (!access_ok(VERIFY_READ, umsg, sizeof(*umsg)) ||
	    __get_user(uaddr, &umsg->msg_name) ||
	    __get_user(kmsg->msg_namelen, &umsg->msg_namelen) ||
	    __get_user(uiov, &umsg->msg_iov) ||
	    __get_user(nr_segs, &umsg->msg_iovlen) ||
	    __get_user(kmsg->msg_control, &umsg->msg_control) ||
	    __get_user(kmsg->msg_controllen, &umsg->msg_controllen) ||
	    __get_user(kmsg->msg_flags, &umsg->msg_flags))
		return -EFAULT;

	if (!uaddr)
		kmsg->msg_namelen = 0;

	if (kmsg->msg_namelen < 0)
		return -EINVAL;

	if (kmsg->msg_namelen > sizeof(struct sockaddr_storage))
		kmsg->msg_namelen = sizeof(struct sockaddr_storage);

	if (save_addr)
		*save_addr = uaddr;

	if (uaddr && kmsg->msg_namelen) {
		if (!save_addr) {
			err = move_addr_to_kernel(uaddr, kmsg->msg_namelen,
						  kmsg->msg_name);
			if (err < 0)
				return err;
		}
	} else {
		kmsg->msg_name = NULL;
		kmsg->msg_namelen = 0;
	}

	if (nr_segs > UIO_MAXIOV)
		return -EMSGSIZE;

	kmsg->msg_iocb = NULL;

	return import_iovec(save_addr ? READ : WRITE, uiov, nr_segs,
			    UIO_FASTIOV, iov, &kmsg->msg_iter);
}
