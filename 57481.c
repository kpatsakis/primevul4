static void ip6_append_data_mtu(unsigned int *mtu,
				int *maxfraglen,
				unsigned int fragheaderlen,
				struct sk_buff *skb,
				struct rt6_info *rt,
				unsigned int orig_mtu)
{
	if (!(rt->dst.flags & DST_XFRM_TUNNEL)) {
		if (!skb) {
			/* first fragment, reserve header_len */
			*mtu = orig_mtu - rt->dst.header_len;

		} else {
			/*
			 * this fragment is not first, the headers
			 * space is regarded as data space.
			 */
			*mtu = orig_mtu;
		}
		*maxfraglen = ((*mtu - fragheaderlen) & ~7)
			      + fragheaderlen - sizeof(struct frag_hdr);
	}
}
