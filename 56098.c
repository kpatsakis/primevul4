rx_ack_print(netdissect_options *ndo,
             register const u_char *bp, int length)
{
	const struct rx_ackPacket *rxa;
	int i, start, last;
	uint32_t firstPacket;

	if (length < (int)sizeof(struct rx_header))
		return;

	bp += sizeof(struct rx_header);

	/*
	 * This may seem a little odd .... the rx_ackPacket structure
	 * contains an array of individual packet acknowledgements
	 * (used for selective ack/nack), but since it's variable in size,
	 * we don't want to truncate based on the size of the whole
	 * rx_ackPacket structure.
	 */

	ND_TCHECK2(bp[0], sizeof(struct rx_ackPacket) - RX_MAXACKS);

	rxa = (const struct rx_ackPacket *) bp;
	bp += (sizeof(struct rx_ackPacket) - RX_MAXACKS);

	/*
	 * Print out a few useful things from the ack packet structure
	 */

	if (ndo->ndo_vflag > 2)
		ND_PRINT((ndo, " bufspace %d maxskew %d",
		       (int) EXTRACT_16BITS(&rxa->bufferSpace),
		       (int) EXTRACT_16BITS(&rxa->maxSkew)));

	firstPacket = EXTRACT_32BITS(&rxa->firstPacket);
	ND_PRINT((ndo, " first %d serial %d reason %s",
	       firstPacket, EXTRACT_32BITS(&rxa->serial),
	       tok2str(rx_ack_reasons, "#%d", (int) rxa->reason)));

	/*
	 * Okay, now we print out the ack array.  The way _this_ works
	 * is that we start at "first", and step through the ack array.
	 * If we have a contiguous range of acks/nacks, try to
	 * collapse them into a range.
	 *
	 * If you're really clever, you might have noticed that this
	 * doesn't seem quite correct.  Specifically, due to structure
	 * padding, sizeof(struct rx_ackPacket) - RX_MAXACKS won't actually
	 * yield the start of the ack array (because RX_MAXACKS is 255
	 * and the structure will likely get padded to a 2 or 4 byte
	 * boundary).  However, this is the way it's implemented inside
	 * of AFS - the start of the extra fields are at
	 * sizeof(struct rx_ackPacket) - RX_MAXACKS + nAcks, which _isn't_
	 * the exact start of the ack array.  Sigh.  That's why we aren't
	 * using bp, but instead use rxa->acks[].  But nAcks gets added
	 * to bp after this, so bp ends up at the right spot.  Go figure.
	 */

	if (rxa->nAcks != 0) {

		ND_TCHECK2(bp[0], rxa->nAcks);

		/*
		 * Sigh, this is gross, but it seems to work to collapse
		 * ranges correctly.
		 */

		for (i = 0, start = last = -2; i < rxa->nAcks; i++)
			if (rxa->acks[i] == RX_ACK_TYPE_ACK) {

				/*
				 * I figured this deserved _some_ explanation.
				 * First, print "acked" and the packet seq
				 * number if this is the first time we've
				 * seen an acked packet.
				 */

				if (last == -2) {
					ND_PRINT((ndo, " acked %d", firstPacket + i));
					start = i;
				}

				/*
				 * Otherwise, if there is a skip in
				 * the range (such as an nacked packet in
				 * the middle of some acked packets),
				 * then print the current packet number
				 * seperated from the last number by
				 * a comma.
				 */

				else if (last != i - 1) {
					ND_PRINT((ndo, ",%d", firstPacket + i));
					start = i;
				}

				/*
				 * We always set last to the value of
				 * the last ack we saw.  Conversely, start
				 * is set to the value of the first ack
				 * we saw in a range.
				 */

				last = i;

				/*
				 * Okay, this bit a code gets executed when
				 * we hit a nack ... in _this_ case we
				 * want to print out the range of packets
				 * that were acked, so we need to print
				 * the _previous_ packet number seperated
				 * from the first by a dash (-).  Since we
				 * already printed the first packet above,
				 * just print the final packet.  Don't
				 * do this if there will be a single-length
				 * range.
				 */
			} else if (last == i - 1 && start != last)
				ND_PRINT((ndo, "-%d", firstPacket + i - 1));

		/*
		 * So, what's going on here?  We ran off the end of the
		 * ack list, and if we got a range we need to finish it up.
		 * So we need to determine if the last packet in the list
		 * was an ack (if so, then last will be set to it) and
		 * we need to see if the last range didn't start with the
		 * last packet (because if it _did_, then that would mean
		 * that the packet number has already been printed and
		 * we don't need to print it again).
		 */

		if (last == i - 1 && start != last)
			ND_PRINT((ndo, "-%d", firstPacket + i - 1));

		/*
		 * Same as above, just without comments
		 */

		for (i = 0, start = last = -2; i < rxa->nAcks; i++)
			if (rxa->acks[i] == RX_ACK_TYPE_NACK) {
				if (last == -2) {
					ND_PRINT((ndo, " nacked %d", firstPacket + i));
					start = i;
				} else if (last != i - 1) {
					ND_PRINT((ndo, ",%d", firstPacket + i));
					start = i;
				}
				last = i;
			} else if (last == i - 1 && start != last)
				ND_PRINT((ndo, "-%d", firstPacket + i - 1));

		if (last == i - 1 && start != last)
			ND_PRINT((ndo, "-%d", firstPacket + i - 1));

		bp += rxa->nAcks;
	}


	/*
	 * These are optional fields; depending on your version of AFS,
	 * you may or may not see them
	 */

#define TRUNCRET(n)	if (ndo->ndo_snapend - bp + 1 <= n) return;

	if (ndo->ndo_vflag > 1) {
		TRUNCRET(4);
		ND_PRINT((ndo, " ifmtu"));
		INTOUT();

		TRUNCRET(4);
		ND_PRINT((ndo, " maxmtu"));
		INTOUT();

		TRUNCRET(4);
		ND_PRINT((ndo, " rwind"));
		INTOUT();

		TRUNCRET(4);
		ND_PRINT((ndo, " maxpackets"));
		INTOUT();
	}

	return;

trunc:
	ND_PRINT((ndo, " [|ack]"));
}
