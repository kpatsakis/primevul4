static void tcp_v6_send_ack(const struct sock *sk, struct sk_buff *skb, u32 seq,
			    u32 ack, u32 win, u32 tsval, u32 tsecr, int oif,
			    struct tcp_md5sig_key *key, u8 tclass,
			    __be32 label)
{
	tcp_v6_send_response(sk, skb, seq, ack, win, tsval, tsecr, oif, key, 0,
			     tclass, label);
}
