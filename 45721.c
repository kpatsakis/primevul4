static int copy_msghdr_from_user(struct msghdr *kmsg,
				 struct msghdr __user *umsg)
{
	if (copy_from_user(kmsg, umsg, sizeof(struct msghdr)))
		return -EFAULT;
	if (kmsg->msg_namelen > sizeof(struct sockaddr_storage))
		return -EINVAL;
	return 0;
}
