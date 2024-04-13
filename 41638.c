SCTP_STATIC int sctp_getsockopt_connectx3(struct sock* sk, int len,
					char __user *optval,
					int __user *optlen)
{
	struct sctp_getaddrs_old param;
	sctp_assoc_t assoc_id = 0;
	int err = 0;

	if (len < sizeof(param))
		return -EINVAL;

	if (copy_from_user(&param, optval, sizeof(param)))
		return -EFAULT;

	err = __sctp_setsockopt_connectx(sk,
			(struct sockaddr __user *)param.addrs,
			param.addr_num, &assoc_id);

	if (err == 0 || err == -EINPROGRESS) {
		if (copy_to_user(optval, &assoc_id, sizeof(assoc_id)))
			return -EFAULT;
		if (put_user(sizeof(assoc_id), optlen))
			return -EFAULT;
	}

	return err;
}
