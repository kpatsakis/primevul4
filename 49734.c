static void tgr192_pass(u64 * ra, u64 * rb, u64 * rc, u64 * x, int mul)
{
	u64 a = *ra;
	u64 b = *rb;
	u64 c = *rc;

	tgr192_round(&a, &b, &c, x[0], mul);
	tgr192_round(&b, &c, &a, x[1], mul);
	tgr192_round(&c, &a, &b, x[2], mul);
	tgr192_round(&a, &b, &c, x[3], mul);
	tgr192_round(&b, &c, &a, x[4], mul);
	tgr192_round(&c, &a, &b, x[5], mul);
	tgr192_round(&a, &b, &c, x[6], mul);
	tgr192_round(&b, &c, &a, x[7], mul);

	*ra = a;
	*rb = b;
	*rc = c;
}
