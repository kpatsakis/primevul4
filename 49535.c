void __cast5_decrypt(struct cast5_ctx *c, u8 *outbuf, const u8 *inbuf)
{
	const __be32 *src = (const __be32 *)inbuf;
	__be32 *dst = (__be32 *)outbuf;
	u32 l, r, t;
	u32 I;
	u32 *Km;
	u8 *Kr;

	Km = c->Km;
	Kr = c->Kr;

	l = be32_to_cpu(src[0]);
	r = be32_to_cpu(src[1]);

	if (!(c->rr)) {
		t = l; l = r; r = t ^ F1(r, Km[15], Kr[15]);
		t = l; l = r; r = t ^ F3(r, Km[14], Kr[14]);
		t = l; l = r; r = t ^ F2(r, Km[13], Kr[13]);
		t = l; l = r; r = t ^ F1(r, Km[12], Kr[12]);
	}
	t = l; l = r; r = t ^ F3(r, Km[11], Kr[11]);
	t = l; l = r; r = t ^ F2(r, Km[10], Kr[10]);
	t = l; l = r; r = t ^ F1(r, Km[9], Kr[9]);
	t = l; l = r; r = t ^ F3(r, Km[8], Kr[8]);
	t = l; l = r; r = t ^ F2(r, Km[7], Kr[7]);
	t = l; l = r; r = t ^ F1(r, Km[6], Kr[6]);
	t = l; l = r; r = t ^ F3(r, Km[5], Kr[5]);
	t = l; l = r; r = t ^ F2(r, Km[4], Kr[4]);
	t = l; l = r; r = t ^ F1(r, Km[3], Kr[3]);
	t = l; l = r; r = t ^ F3(r, Km[2], Kr[2]);
	t = l; l = r; r = t ^ F2(r, Km[1], Kr[1]);
	t = l; l = r; r = t ^ F1(r, Km[0], Kr[0]);

	dst[0] = cpu_to_be32(r);
	dst[1] = cpu_to_be32(l);
}
