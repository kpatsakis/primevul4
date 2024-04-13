int ip6_append_data(struct sock *sk,
		    int getfrag(void *from, char *to, int offset, int len,
				int odd, struct sk_buff *skb),
		    void *from, int length, int transhdrlen,
		    struct ipcm6_cookie *ipc6, struct flowi6 *fl6,
		    struct rt6_info *rt, unsigned int flags,
		    const struct sockcm_cookie *sockc)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ipv6_pinfo *np = inet6_sk(sk);
	int exthdrlen;
	int err;

	if (flags&MSG_PROBE)
		return 0;
	if (skb_queue_empty(&sk->sk_write_queue)) {
		/*
		 * setup for corking
		 */
		err = ip6_setup_cork(sk, &inet->cork, &np->cork,
				     ipc6, rt, fl6);
		if (err)
			return err;

		exthdrlen = (ipc6->opt ? ipc6->opt->opt_flen : 0);
		length += exthdrlen;
		transhdrlen += exthdrlen;
	} else {
		fl6 = &inet->cork.fl.u.ip6;
		transhdrlen = 0;
	}

	return __ip6_append_data(sk, fl6, &sk->sk_write_queue, &inet->cork.base,
				 &np->cork, sk_page_frag(sk), getfrag,
				 from, length, transhdrlen, flags, ipc6, sockc);
}
