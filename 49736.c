static void tgr192_transform(struct tgr192_ctx *tctx, const u8 * data)
{
	u64 a, b, c, aa, bb, cc;
	u64 x[8];
	int i;
	const __le64 *ptr = (const __le64 *)data;

	for (i = 0; i < 8; i++)
		x[i] = le64_to_cpu(ptr[i]);

	/* save */
	a = aa = tctx->a;
	b = bb = tctx->b;
	c = cc = tctx->c;

	tgr192_pass(&a, &b, &c, x, 5);
	tgr192_key_schedule(x);
	tgr192_pass(&c, &a, &b, x, 7);
	tgr192_key_schedule(x);
	tgr192_pass(&b, &c, &a, x, 9);


	/* feedforward */
	a ^= aa;
	b -= bb;
	c += cc;
	/* store */
	tctx->a = a;
	tctx->b = b;
	tctx->c = c;
}
