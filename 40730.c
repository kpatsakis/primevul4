static struct sctp_sackhdr *sctp_sm_pull_sack(struct sctp_chunk *chunk)
{
	struct sctp_sackhdr *sack;
	unsigned int len;
	__u16 num_blocks;
	__u16 num_dup_tsns;

	/* Protect ourselves from reading too far into
	 * the skb from a bogus sender.
	 */
	sack = (struct sctp_sackhdr *) chunk->skb->data;

	num_blocks = ntohs(sack->num_gap_ack_blocks);
	num_dup_tsns = ntohs(sack->num_dup_tsns);
	len = sizeof(struct sctp_sackhdr);
	len += (num_blocks + num_dup_tsns) * sizeof(__u32);
	if (len > chunk->skb->len)
		return NULL;

	skb_pull(chunk->skb, len);

	return sack;
}
