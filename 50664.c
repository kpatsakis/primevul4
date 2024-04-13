static int fanout_set_data_cbpf(struct packet_sock *po, char __user *data,
				unsigned int len)
{
	struct bpf_prog *new;
	struct sock_fprog fprog;
	int ret;

	if (sock_flag(&po->sk, SOCK_FILTER_LOCKED))
		return -EPERM;
	if (len != sizeof(fprog))
		return -EINVAL;
	if (copy_from_user(&fprog, data, len))
		return -EFAULT;

	ret = bpf_prog_create_from_user(&new, &fprog, NULL, false);
	if (ret)
		return ret;

	__fanout_set_data_bpf(po->fanout, new);
	return 0;
}
