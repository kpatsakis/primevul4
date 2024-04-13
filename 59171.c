static void dccp_rcv_reset(struct sock *sk, struct sk_buff *skb)
{
	u16 err = dccp_reset_code_convert(dccp_hdr_reset(skb)->dccph_reset_code);

	sk->sk_err = err;

	/* Queue the equivalent of TCP fin so that dccp_recvmsg exits the loop */
	dccp_fin(sk, skb);

	if (err && !sock_flag(sk, SOCK_DEAD))
		sk_wake_async(sk, SOCK_WAKE_IO, POLL_ERR);
	dccp_time_wait(sk, DCCP_TIME_WAIT, 0);
}
