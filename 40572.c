static int pfkey_broadcast_one(struct sk_buff *skb, struct sk_buff **skb2,
			       gfp_t allocation, struct sock *sk)
{
	int err = -ENOBUFS;

	sock_hold(sk);
	if (*skb2 == NULL) {
		if (atomic_read(&skb->users) != 1) {
			*skb2 = skb_clone(skb, allocation);
		} else {
			*skb2 = skb;
			atomic_inc(&skb->users);
		}
	}
	if (*skb2 != NULL) {
		if (atomic_read(&sk->sk_rmem_alloc) <= sk->sk_rcvbuf) {
			skb_set_owner_r(*skb2, sk);
			skb_queue_tail(&sk->sk_receive_queue, *skb2);
			sk->sk_data_ready(sk, (*skb2)->len);
			*skb2 = NULL;
			err = 0;
		}
	}
	sock_put(sk);
	return err;
}
