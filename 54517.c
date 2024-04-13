static void raw_close(struct sock *sk, long timeout)
{
	/*
	 * Raw sockets may have direct kernel references. Kill them.
	 */
	rtnl_lock();
	ip_ra_control(sk, 0, NULL);
	rtnl_unlock();

	sk_common_release(sk);
}
