static int skb_checksum_setup_ipv4(struct sk_buff *skb, bool recalculate)
{
	unsigned int off;
	bool fragment;
	__sum16 *csum;
	int err;

	fragment = false;

	err = skb_maybe_pull_tail(skb,
				  sizeof(struct iphdr),
				  MAX_IP_HDR_LEN);
	if (err < 0)
		goto out;

	if (ip_hdr(skb)->frag_off & htons(IP_OFFSET | IP_MF))
		fragment = true;

	off = ip_hdrlen(skb);

	err = -EPROTO;

	if (fragment)
		goto out;

	csum = skb_checksum_setup_ip(skb, ip_hdr(skb)->protocol, off);
	if (IS_ERR(csum))
		return PTR_ERR(csum);

	if (recalculate)
		*csum = ~csum_tcpudp_magic(ip_hdr(skb)->saddr,
					   ip_hdr(skb)->daddr,
					   skb->len - off,
					   ip_hdr(skb)->protocol, 0);
	err = 0;

out:
	return err;
}
