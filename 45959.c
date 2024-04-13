void sock_edemux(struct sk_buff *skb)
{
	struct sock *sk = skb->sk;

#ifdef CONFIG_INET
	if (sk->sk_state == TCP_TIME_WAIT)
		inet_twsk_put(inet_twsk(sk));
	else
#endif
		sock_put(sk);
}
