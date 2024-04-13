static int tcp_sendmsg_fastopen(struct sock *sk, struct msghdr *msg,
				int *copied, size_t size)
{
	struct tcp_sock *tp = tcp_sk(sk);
	int err, flags;

	if (!(sysctl_tcp_fastopen & TFO_CLIENT_ENABLE))
		return -EOPNOTSUPP;
	if (tp->fastopen_req)
		return -EALREADY; /* Another Fast Open is in progress */

	tp->fastopen_req = kzalloc(sizeof(struct tcp_fastopen_request),
				   sk->sk_allocation);
	if (unlikely(!tp->fastopen_req))
		return -ENOBUFS;
	tp->fastopen_req->data = msg;
	tp->fastopen_req->size = size;

	flags = (msg->msg_flags & MSG_DONTWAIT) ? O_NONBLOCK : 0;
	err = __inet_stream_connect(sk->sk_socket, msg->msg_name,
				    msg->msg_namelen, flags);
	*copied = tp->fastopen_req->copied;
	tcp_free_fastopen_req(tp);
	return err;
}
