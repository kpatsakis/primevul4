static int ip6_setup_cork(struct sock *sk, struct inet_cork_full *cork,
			  struct inet6_cork *v6_cork, struct ipcm6_cookie *ipc6,
			  struct rt6_info *rt, struct flowi6 *fl6)
{
	struct ipv6_pinfo *np = inet6_sk(sk);
	unsigned int mtu;
	struct ipv6_txoptions *opt = ipc6->opt;

	/*
	 * setup for corking
	 */
	if (opt) {
		if (WARN_ON(v6_cork->opt))
			return -EINVAL;

		v6_cork->opt = kzalloc(opt->tot_len, sk->sk_allocation);
		if (unlikely(!v6_cork->opt))
			return -ENOBUFS;

		v6_cork->opt->tot_len = opt->tot_len;
		v6_cork->opt->opt_flen = opt->opt_flen;
		v6_cork->opt->opt_nflen = opt->opt_nflen;

		v6_cork->opt->dst0opt = ip6_opt_dup(opt->dst0opt,
						    sk->sk_allocation);
		if (opt->dst0opt && !v6_cork->opt->dst0opt)
			return -ENOBUFS;

		v6_cork->opt->dst1opt = ip6_opt_dup(opt->dst1opt,
						    sk->sk_allocation);
		if (opt->dst1opt && !v6_cork->opt->dst1opt)
			return -ENOBUFS;

		v6_cork->opt->hopopt = ip6_opt_dup(opt->hopopt,
						   sk->sk_allocation);
		if (opt->hopopt && !v6_cork->opt->hopopt)
			return -ENOBUFS;

		v6_cork->opt->srcrt = ip6_rthdr_dup(opt->srcrt,
						    sk->sk_allocation);
		if (opt->srcrt && !v6_cork->opt->srcrt)
			return -ENOBUFS;

		/* need source address above miyazawa*/
	}
	dst_hold(&rt->dst);
	cork->base.dst = &rt->dst;
	cork->fl.u.ip6 = *fl6;
	v6_cork->hop_limit = ipc6->hlimit;
	v6_cork->tclass = ipc6->tclass;
	if (rt->dst.flags & DST_XFRM_TUNNEL)
		mtu = np->pmtudisc >= IPV6_PMTUDISC_PROBE ?
		      rt->dst.dev->mtu : dst_mtu(&rt->dst);
	else
		mtu = np->pmtudisc >= IPV6_PMTUDISC_PROBE ?
		      rt->dst.dev->mtu : dst_mtu(rt->dst.path);
	if (np->frag_size < mtu) {
		if (np->frag_size)
			mtu = np->frag_size;
	}
	cork->base.fragsize = mtu;
	if (dst_allfrag(rt->dst.path))
		cork->base.flags |= IPCORK_ALLFRAG;
	cork->base.length = 0;

	return 0;
}
