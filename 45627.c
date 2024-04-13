static int nfc_llcp_getsockopt(struct socket *sock, int level, int optname,
			       char __user *optval, int __user *optlen)
{
	struct nfc_llcp_local *local;
	struct sock *sk = sock->sk;
	struct nfc_llcp_sock *llcp_sock = nfc_llcp_sock(sk);
	int len, err = 0;
	u16 miux, remote_miu;
	u8 rw;

	pr_debug("%p optname %d\n", sk, optname);

	if (level != SOL_NFC)
		return -ENOPROTOOPT;

	if (get_user(len, optlen))
		return -EFAULT;

	local = llcp_sock->local;
	if (!local)
		return -ENODEV;

	len = min_t(u32, len, sizeof(u32));

	lock_sock(sk);

	switch (optname) {
	case NFC_LLCP_RW:
		rw = llcp_sock->rw > LLCP_MAX_RW ? local->rw : llcp_sock->rw;
		if (put_user(rw, (u32 __user *) optval))
			err = -EFAULT;

		break;

	case NFC_LLCP_MIUX:
		miux = be16_to_cpu(llcp_sock->miux) > LLCP_MAX_MIUX ?
			be16_to_cpu(local->miux) : be16_to_cpu(llcp_sock->miux);

		if (put_user(miux, (u32 __user *) optval))
			err = -EFAULT;

		break;

	case NFC_LLCP_REMOTE_MIU:
		remote_miu = llcp_sock->remote_miu > LLCP_MAX_MIU ?
				local->remote_miu : llcp_sock->remote_miu;

		if (put_user(remote_miu, (u32 __user *) optval))
			err = -EFAULT;

		break;

	case NFC_LLCP_REMOTE_LTO:
		if (put_user(local->remote_lto / 10, (u32 __user *) optval))
			err = -EFAULT;

		break;

	case NFC_LLCP_REMOTE_RW:
		if (put_user(llcp_sock->remote_rw, (u32 __user *) optval))
			err = -EFAULT;

		break;

	default:
		err = -ENOPROTOOPT;
		break;
	}

	release_sock(sk);

	if (put_user(len, optlen))
		return -EFAULT;

	return err;
}
