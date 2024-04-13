static int compat_packet_setsockopt(struct socket *sock, int level, int optname,
				    char __user *optval, unsigned int optlen)
{
	struct packet_sock *po = pkt_sk(sock->sk);

	if (level != SOL_PACKET)
		return -ENOPROTOOPT;

	if (optname == PACKET_FANOUT_DATA &&
	    po->fanout && po->fanout->type == PACKET_FANOUT_CBPF) {
		optval = (char __user *)get_compat_bpf_fprog(optval);
		if (!optval)
			return -EFAULT;
		optlen = sizeof(struct sock_fprog);
	}

	return packet_setsockopt(sock, level, optname, optval, optlen);
}
