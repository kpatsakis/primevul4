struct sk_buff *skb_clone_sk(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;
	struct sk_buff *clone;

	if (!sk || !atomic_inc_not_zero(&sk->sk_refcnt))
		return NULL;

	clone = skb_clone(skb, GFP_ATOMIC);
	if (!clone) {
		sock_put(sk);
		return NULL;
	}

	clone->sk = sk;
	clone->destructor = sock_efree;

	return clone;
}
