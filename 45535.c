unsigned int iucv_sock_poll(struct file *file, struct socket *sock,
			    poll_table *wait)
{
	struct sock *sk = sock->sk;
	unsigned int mask = 0;

	sock_poll_wait(file, sk_sleep(sk), wait);

	if (sk->sk_state == IUCV_LISTEN)
		return iucv_accept_poll(sk);

	if (sk->sk_err || !skb_queue_empty(&sk->sk_error_queue))
		mask |= POLLERR |
			(sock_flag(sk, SOCK_SELECT_ERR_QUEUE) ? POLLPRI : 0);

	if (sk->sk_shutdown & RCV_SHUTDOWN)
		mask |= POLLRDHUP;

	if (sk->sk_shutdown == SHUTDOWN_MASK)
		mask |= POLLHUP;

	if (!skb_queue_empty(&sk->sk_receive_queue) ||
	    (sk->sk_shutdown & RCV_SHUTDOWN))
		mask |= POLLIN | POLLRDNORM;

	if (sk->sk_state == IUCV_CLOSED)
		mask |= POLLHUP;

	if (sk->sk_state == IUCV_DISCONN)
		mask |= POLLIN;

	if (sock_writeable(sk) && iucv_below_msglim(sk))
		mask |= POLLOUT | POLLWRNORM | POLLWRBAND;
	else
		set_bit(SOCK_ASYNC_NOSPACE, &sk->sk_socket->flags);

	return mask;
}
