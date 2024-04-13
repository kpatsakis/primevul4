static unsigned int llcp_sock_poll(struct file *file, struct socket *sock,
				   poll_table *wait)
{
	struct sock *sk = sock->sk;
	unsigned int mask = 0;

	pr_debug("%p\n", sk);

	sock_poll_wait(file, sk_sleep(sk), wait);

	if (sk->sk_state == LLCP_LISTEN)
		return llcp_accept_poll(sk);

	if (sk->sk_err || !skb_queue_empty(&sk->sk_error_queue))
		mask |= POLLERR |
			(sock_flag(sk, SOCK_SELECT_ERR_QUEUE) ? POLLPRI : 0);

	if (!skb_queue_empty(&sk->sk_receive_queue))
		mask |= POLLIN | POLLRDNORM;

	if (sk->sk_state == LLCP_CLOSED)
		mask |= POLLHUP;

	if (sk->sk_shutdown & RCV_SHUTDOWN)
		mask |= POLLRDHUP | POLLIN | POLLRDNORM;

	if (sk->sk_shutdown == SHUTDOWN_MASK)
		mask |= POLLHUP;

	if (sock_writeable(sk) && sk->sk_state == LLCP_CONNECTED)
		mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
	else
		set_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags);

	pr_debug("mask 0x%x\n", mask);

	return mask;
}
