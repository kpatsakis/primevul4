static int fanout_set_data_ebpf(struct packet_sock *po, char __user *data,
				unsigned int len)
{
	struct bpf_prog *new;
	u32 fd;

	if (sock_flag(&po->sk, SOCK_FILTER_LOCKED))
		return -EPERM;
	if (len != sizeof(fd))
		return -EINVAL;
	if (copy_from_user(&fd, data, len))
		return -EFAULT;

	new = bpf_prog_get_type(fd, BPF_PROG_TYPE_SOCKET_FILTER);
	if (IS_ERR(new))
		return PTR_ERR(new);

	__fanout_set_data_bpf(po->fanout, new);
	return 0;
}
