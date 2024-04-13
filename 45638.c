static void rawsock_report_error(struct sock *sk, int err)
{
	pr_debug("sk=%p err=%d\n", sk, err);

	sk->sk_shutdown = SHUTDOWN_MASK;
	sk->sk_err = -err;
	sk->sk_error_report(sk);

	rawsock_write_queue_purge(sk);
}
