struct ndisc_options *ndisc_parse_options(u8 *opt, int opt_len,
					  struct ndisc_options *ndopts)
{
	struct nd_opt_hdr *nd_opt = (struct nd_opt_hdr *)opt;

	if (!nd_opt || opt_len < 0 || !ndopts)
		return NULL;
	memset(ndopts, 0, sizeof(*ndopts));
	while (opt_len) {
		int l;
		if (opt_len < sizeof(struct nd_opt_hdr))
			return NULL;
		l = nd_opt->nd_opt_len << 3;
		if (opt_len < l || l == 0)
			return NULL;
		switch (nd_opt->nd_opt_type) {
		case ND_OPT_SOURCE_LL_ADDR:
		case ND_OPT_TARGET_LL_ADDR:
		case ND_OPT_MTU:
		case ND_OPT_REDIRECT_HDR:
			if (ndopts->nd_opt_array[nd_opt->nd_opt_type]) {
				ND_PRINTK(2, warn,
					  "%s: duplicated ND6 option found: type=%d\n",
					  __func__, nd_opt->nd_opt_type);
			} else {
				ndopts->nd_opt_array[nd_opt->nd_opt_type] = nd_opt;
			}
			break;
		case ND_OPT_PREFIX_INFO:
			ndopts->nd_opts_pi_end = nd_opt;
			if (!ndopts->nd_opt_array[nd_opt->nd_opt_type])
				ndopts->nd_opt_array[nd_opt->nd_opt_type] = nd_opt;
			break;
#ifdef CONFIG_IPV6_ROUTE_INFO
		case ND_OPT_ROUTE_INFO:
			ndopts->nd_opts_ri_end = nd_opt;
			if (!ndopts->nd_opts_ri)
				ndopts->nd_opts_ri = nd_opt;
			break;
#endif
		default:
			if (ndisc_is_useropt(nd_opt)) {
				ndopts->nd_useropts_end = nd_opt;
				if (!ndopts->nd_useropts)
					ndopts->nd_useropts = nd_opt;
			} else {
				/*
				 * Unknown options must be silently ignored,
				 * to accommodate future extension to the
				 * protocol.
				 */
				ND_PRINTK(2, notice,
					  "%s: ignored unsupported option; type=%d, len=%d\n",
					  __func__,
					  nd_opt->nd_opt_type,
					  nd_opt->nd_opt_len);
			}
		}
		opt_len -= l;
		nd_opt = ((void *)nd_opt) + l;
	}
	return ndopts;
}
