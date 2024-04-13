static inline int sctp_rcv_checksum(struct sk_buff *skb)
{
	struct sk_buff *list = skb_shinfo(skb)->frag_list;
	struct sctphdr *sh = sctp_hdr(skb);
	__be32 cmp = sh->checksum;
	__be32 val = sctp_start_cksum((__u8 *)sh, skb_headlen(skb));

	for (; list; list = list->next)
		val = sctp_update_cksum((__u8 *)list->data, skb_headlen(list),
					val);

	val = sctp_end_cksum(val);

	if (val != cmp) {
		/* CRC failure, dump it. */
		SCTP_INC_STATS_BH(SCTP_MIB_CHECKSUMERRORS);
		return -1;
	}
	return 0;
}
