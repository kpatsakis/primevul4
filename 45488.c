__be16 ipx_cksum(struct ipxhdr *packet, int length)
{
	/*
	 *	NOTE: sum is a net byte order quantity, which optimizes the
	 *	loop. This only works on big and little endian machines. (I
	 *	don't know of a machine that isn't.)
	 */
	/* handle the first 3 words separately; checksum should be skipped
	 * and ipx_tctrl masked out */
	__u16 *p = (__u16 *)packet;
	__u32 sum = p[1] + (p[2] & (__force u16)htons(0x00ff));
	__u32 i = (length >> 1) - 3; /* Number of remaining complete words */

	/* Loop through them */
	p += 3;
	while (i--)
		sum += *p++;

	/* Add on the last part word if it exists */
	if (packet->ipx_pktsize & htons(1))
		sum += (__force u16)htons(0xff00) & *p;

	/* Do final fixup */
	sum = (sum & 0xffff) + (sum >> 16);

	/* It's a pity there's no concept of carry in C */
	if (sum >= 0x10000)
		sum++;

	/*
	 * Leave 0 alone; we don't want 0xffff here.  Note that we can't get
	 * here with 0x10000, so this check is the same as ((__u16)sum)
	 */
	if (sum)
		sum = ~sum;

	return (__force __be16)sum;
}
