SCTP_STATIC int sctp_setsockopt_connectx(struct sock* sk,
				      struct sockaddr __user *addrs,
				      int addrs_size)
{
	int err = 0;
	struct sockaddr *kaddrs;

	SCTP_DEBUG_PRINTK("%s - sk %p addrs %p addrs_size %d\n",
			  __FUNCTION__, sk, addrs, addrs_size);

	if (unlikely(addrs_size <= 0))
		return -EINVAL;

	/* Check the user passed a healthy pointer.  */
	if (unlikely(!access_ok(VERIFY_READ, addrs, addrs_size)))
		return -EFAULT;

	/* Alloc space for the address array in kernel memory.  */
	kaddrs = kmalloc(addrs_size, GFP_KERNEL);
	if (unlikely(!kaddrs))
		return -ENOMEM;

	if (__copy_from_user(kaddrs, addrs, addrs_size)) {
		err = -EFAULT;
	} else {
		err = __sctp_connect(sk, kaddrs, addrs_size);
	}

	kfree(kaddrs);
	return err;
}
