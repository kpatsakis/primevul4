static int md_set_badblocks(struct badblocks *bb, sector_t s, int sectors,
			    int acknowledged)
{
	u64 *p;
	int lo, hi;
	int rv = 1;
	unsigned long flags;

	if (bb->shift < 0)
		/* badblocks are disabled */
		return 0;

	if (bb->shift) {
		/* round the start down, and the end up */
		sector_t next = s + sectors;
		s >>= bb->shift;
		next += (1<<bb->shift) - 1;
		next >>= bb->shift;
		sectors = next - s;
	}

	write_seqlock_irqsave(&bb->lock, flags);

	p = bb->page;
	lo = 0;
	hi = bb->count;
	/* Find the last range that starts at-or-before 's' */
	while (hi - lo > 1) {
		int mid = (lo + hi) / 2;
		sector_t a = BB_OFFSET(p[mid]);
		if (a <= s)
			lo = mid;
		else
			hi = mid;
	}
	if (hi > lo && BB_OFFSET(p[lo]) > s)
		hi = lo;

	if (hi > lo) {
		/* we found a range that might merge with the start
		 * of our new range
		 */
		sector_t a = BB_OFFSET(p[lo]);
		sector_t e = a + BB_LEN(p[lo]);
		int ack = BB_ACK(p[lo]);
		if (e >= s) {
			/* Yes, we can merge with a previous range */
			if (s == a && s + sectors >= e)
				/* new range covers old */
				ack = acknowledged;
			else
				ack = ack && acknowledged;

			if (e < s + sectors)
				e = s + sectors;
			if (e - a <= BB_MAX_LEN) {
				p[lo] = BB_MAKE(a, e-a, ack);
				s = e;
			} else {
				/* does not all fit in one range,
				 * make p[lo] maximal
				 */
				if (BB_LEN(p[lo]) != BB_MAX_LEN)
					p[lo] = BB_MAKE(a, BB_MAX_LEN, ack);
				s = a + BB_MAX_LEN;
			}
			sectors = e - s;
		}
	}
	if (sectors && hi < bb->count) {
		/* 'hi' points to the first range that starts after 's'.
		 * Maybe we can merge with the start of that range */
		sector_t a = BB_OFFSET(p[hi]);
		sector_t e = a + BB_LEN(p[hi]);
		int ack = BB_ACK(p[hi]);
		if (a <= s + sectors) {
			/* merging is possible */
			if (e <= s + sectors) {
				/* full overlap */
				e = s + sectors;
				ack = acknowledged;
			} else
				ack = ack && acknowledged;

			a = s;
			if (e - a <= BB_MAX_LEN) {
				p[hi] = BB_MAKE(a, e-a, ack);
				s = e;
			} else {
				p[hi] = BB_MAKE(a, BB_MAX_LEN, ack);
				s = a + BB_MAX_LEN;
			}
			sectors = e - s;
			lo = hi;
			hi++;
		}
	}
	if (sectors == 0 && hi < bb->count) {
		/* we might be able to combine lo and hi */
		/* Note: 's' is at the end of 'lo' */
		sector_t a = BB_OFFSET(p[hi]);
		int lolen = BB_LEN(p[lo]);
		int hilen = BB_LEN(p[hi]);
		int newlen = lolen + hilen - (s - a);
		if (s >= a && newlen < BB_MAX_LEN) {
			/* yes, we can combine them */
			int ack = BB_ACK(p[lo]) && BB_ACK(p[hi]);
			p[lo] = BB_MAKE(BB_OFFSET(p[lo]), newlen, ack);
			memmove(p + hi, p + hi + 1,
				(bb->count - hi - 1) * 8);
			bb->count--;
		}
	}
	while (sectors) {
		/* didn't merge (it all).
		 * Need to add a range just before 'hi' */
		if (bb->count >= MD_MAX_BADBLOCKS) {
			/* No room for more */
			rv = 0;
			break;
		} else {
			int this_sectors = sectors;
			memmove(p + hi + 1, p + hi,
				(bb->count - hi) * 8);
			bb->count++;

			if (this_sectors > BB_MAX_LEN)
				this_sectors = BB_MAX_LEN;
			p[hi] = BB_MAKE(s, this_sectors, acknowledged);
			sectors -= this_sectors;
			s += this_sectors;
		}
	}

	bb->changed = 1;
	if (!acknowledged)
		bb->unacked_exist = 1;
	write_sequnlock_irqrestore(&bb->lock, flags);

	return rv;
}
