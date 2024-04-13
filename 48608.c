static u64 l3hash(u64 p1, u64 p2, u64 k1, u64 k2, u64 len)
{
	u64 rh, rl, t, z = 0;

	/* fully reduce (p1,p2)+(len,0) mod p127 */
	t = p1 >> 63;
	p1 &= m63;
	ADD128(p1, p2, len, t);
	/* At this point, (p1,p2) is at most 2^127+(len<<64) */
	t = (p1 > m63) + ((p1 == m63) && (p2 == m64));
	ADD128(p1, p2, z, t);
	p1 &= m63;

	/* compute (p1,p2)/(2^64-2^32) and (p1,p2)%(2^64-2^32) */
	t = p1 + (p2 >> 32);
	t += (t >> 32);
	t += (u32)t > 0xfffffffeu;
	p1 += (t >> 32);
	p2 += (p1 << 32);

	/* compute (p1+k1)%p64 and (p2+k2)%p64 */
	p1 += k1;
	p1 += (0 - (p1 < k1)) & 257;
	p2 += k2;
	p2 += (0 - (p2 < k2)) & 257;

	/* compute (p1+k1)*(p2+k2)%p64 */
	MUL64(rh, rl, p1, p2);
	t = rh >> 56;
	ADD128(t, rl, z, rh);
	rh <<= 8;
	ADD128(t, rl, z, rh);
	t += t << 8;
	rl += t;
	rl += (0 - (rl < t)) & 257;
	rl += (0 - (rl > p64-1)) & 257;
	return rl;
}
