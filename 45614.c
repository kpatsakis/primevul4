static int netlink_unicast_kernel(struct sock *sk, struct sk_buff *skb,
				  struct sock *ssk)
{
	int ret;
	struct netlink_sock *nlk = nlk_sk(sk);

	ret = -ECONNREFUSED;
	if (nlk->netlink_rcv != NULL) {
		/* We could do a netlink_deliver_tap(skb) here as well
		 * but since this is intended for the kernel only, we
		 * should rather let it stay under the hood.
		 */

		ret = skb->len;
		netlink_skb_set_owner_r(skb, sk);
		NETLINK_CB(skb).sk = ssk;
		nlk->netlink_rcv(skb);
		consume_skb(skb);
	} else {
		kfree_skb(skb);
	}
	sock_put(sk);
	return ret;
}
