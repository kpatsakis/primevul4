static bool skb_may_tx_timestamp(struct sock *sk, bool tsonly)
{
	bool ret;

	if (likely(sysctl_tstamp_allow_data || tsonly))
		return true;

	read_lock_bh(&sk->sk_callback_lock);
	ret = sk->sk_socket && sk->sk_socket->file &&
	      file_ns_capable(sk->sk_socket->file, &init_user_ns, CAP_NET_RAW);
	read_unlock_bh(&sk->sk_callback_lock);
	return ret;
}
