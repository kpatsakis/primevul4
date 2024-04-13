static void tnepres_decrypt(struct crypto_tfm *tfm, u8 *dst, const u8 *src)
{
	const u32 * const s = (const u32 * const)src;
	u32 * const d = (u32 * const)dst;

	u32 rs[4], rd[4];

	rs[0] = swab32(s[3]);
	rs[1] = swab32(s[2]);
	rs[2] = swab32(s[1]);
	rs[3] = swab32(s[0]);

	serpent_decrypt(tfm, (u8 *)rd, (u8 *)rs);

	d[0] = swab32(rd[3]);
	d[1] = swab32(rd[2]);
	d[2] = swab32(rd[1]);
	d[3] = swab32(rd[0]);
}
