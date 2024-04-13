int sock_common_recvmsg(struct socket *sock, struct msghdr *msg, size_t size,
			int flags)
{
	struct sock *sk = sock->sk;
	int addr_len = 0;
	int err;

	err = sk->sk_prot->recvmsg(sk, msg, size, flags & MSG_DONTWAIT,
				   flags & ~MSG_DONTWAIT, &addr_len);
	if (err >= 0)
		msg->msg_namelen = addr_len;
	return err;
}
