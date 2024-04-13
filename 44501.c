static bool dccp_pkt_to_tuple(const struct sk_buff *skb, unsigned int dataoff,
			      struct nf_conntrack_tuple *tuple)
{
	struct dccp_hdr _hdr, *dh;

	dh = skb_header_pointer(skb, dataoff, sizeof(_hdr), &_hdr);
	if (dh == NULL)
		return false;

	tuple->src.u.dccp.port = dh->dccph_sport;
	tuple->dst.u.dccp.port = dh->dccph_dport;
	return true;
}
