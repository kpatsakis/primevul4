static void dccp_fin(struct sock *sk, struct sk_buff *skb)
{
	/*
	 * On receiving Close/CloseReq, both RD/WR shutdown are performed.
	 * RFC 4340, 8.3 says that we MAY send further Data/DataAcks after
	 * receiving the closing segment, but there is no guarantee that such
	 * data will be processed at all.
	 */
	sk->sk_shutdown = SHUTDOWN_MASK;
	sock_set_flag(sk, SOCK_DONE);
	dccp_enqueue_skb(sk, skb);
}
