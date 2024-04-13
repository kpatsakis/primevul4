static void ip6_cork_release(struct inet_cork_full *cork,
			     struct inet6_cork *v6_cork)
{
	if (v6_cork->opt) {
		kfree(v6_cork->opt->dst0opt);
		kfree(v6_cork->opt->dst1opt);
		kfree(v6_cork->opt->hopopt);
		kfree(v6_cork->opt->srcrt);
		kfree(v6_cork->opt);
		v6_cork->opt = NULL;
	}

	if (cork->base.dst) {
		dst_release(cork->base.dst);
		cork->base.dst = NULL;
		cork->base.flags &= ~IPCORK_ALLFRAG;
	}
	memset(&cork->fl, 0, sizeof(cork->fl));
}
