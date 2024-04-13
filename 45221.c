int get_compat_msghdr(struct msghdr *kmsg, struct compat_msghdr __user *umsg)
{
	compat_uptr_t tmp1, tmp2, tmp3;

	if (!access_ok(VERIFY_READ, umsg, sizeof(*umsg)) ||
	    __get_user(tmp1, &umsg->msg_name) ||
	    __get_user(kmsg->msg_namelen, &umsg->msg_namelen) ||
	    __get_user(tmp2, &umsg->msg_iov) ||
	    __get_user(kmsg->msg_iovlen, &umsg->msg_iovlen) ||
	    __get_user(tmp3, &umsg->msg_control) ||
	    __get_user(kmsg->msg_controllen, &umsg->msg_controllen) ||
	    __get_user(kmsg->msg_flags, &umsg->msg_flags))
		return -EFAULT;
	if (kmsg->msg_namelen > sizeof(struct sockaddr_storage))
		kmsg->msg_namelen = sizeof(struct sockaddr_storage);
	kmsg->msg_name = compat_ptr(tmp1);
	kmsg->msg_iov = compat_ptr(tmp2);
	kmsg->msg_control = compat_ptr(tmp3);
	return 0;
}
