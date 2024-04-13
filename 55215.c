static int __sctp_setsockopt_connectx(struct sock *sk,
				      struct sockaddr __user *addrs,
				      int addrs_size,
				      sctp_assoc_t *assoc_id)
{
	struct sockaddr *kaddrs;
	gfp_t gfp = GFP_KERNEL;
	int err = 0;

	pr_debug("%s: sk:%p addrs:%p addrs_size:%d\n",
		 __func__, sk, addrs, addrs_size);

	if (unlikely(addrs_size <= 0))
		return -EINVAL;

	/* Check the user passed a healthy pointer.  */
	if (unlikely(!access_ok(VERIFY_READ, addrs, addrs_size)))
		return -EFAULT;

	/* Alloc space for the address array in kernel memory.  */
	if (sk->sk_socket->file)
		gfp = GFP_USER | __GFP_NOWARN;
	kaddrs = kmalloc(addrs_size, gfp);
	if (unlikely(!kaddrs))
		return -ENOMEM;

	if (__copy_from_user(kaddrs, addrs, addrs_size)) {
		err = -EFAULT;
	} else {
		err = __sctp_connect(sk, kaddrs, addrs_size, assoc_id);
	}

	kfree(kaddrs);

	return err;
}
