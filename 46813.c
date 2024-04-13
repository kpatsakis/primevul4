static int udp_v6_push_pending_frames(struct sock *sk)
{
	struct sk_buff *skb;
	struct udp_sock  *up = udp_sk(sk);
	struct flowi6 fl6;
	int err = 0;

	if (up->pending == AF_INET)
		return udp_push_pending_frames(sk);

	/* ip6_finish_skb will release the cork, so make a copy of
	 * fl6 here.
	 */
	fl6 = inet_sk(sk)->cork.fl.u.ip6;

	skb = ip6_finish_skb(sk);
	if (!skb)
		goto out;

	err = udp_v6_send_skb(skb, &fl6);

out:
	up->len = 0;
	up->pending = 0;
	return err;
}
