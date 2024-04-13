static void poly_step_func(u64 *ahi, u64 *alo,
			const u64 *kh, const u64 *kl,
			const u64 *mh, const u64 *ml)
{
#define a0 (*(((u32 *)alo)+INDEX_LOW))
#define a1 (*(((u32 *)alo)+INDEX_HIGH))
#define a2 (*(((u32 *)ahi)+INDEX_LOW))
#define a3 (*(((u32 *)ahi)+INDEX_HIGH))
#define k0 (*(((u32 *)kl)+INDEX_LOW))
#define k1 (*(((u32 *)kl)+INDEX_HIGH))
#define k2 (*(((u32 *)kh)+INDEX_LOW))
#define k3 (*(((u32 *)kh)+INDEX_HIGH))

	u64 p, q, t;
	u32 t2;

	p = MUL32(a3, k3);
	p += p;
	p += *(u64 *)mh;
	p += MUL32(a0, k2);
	p += MUL32(a1, k1);
	p += MUL32(a2, k0);
	t = (u32)(p);
	p >>= 32;
	p += MUL32(a0, k3);
	p += MUL32(a1, k2);
	p += MUL32(a2, k1);
	p += MUL32(a3, k0);
	t |= ((u64)((u32)p & 0x7fffffff)) << 32;
	p >>= 31;
	p += (u64)(((u32 *)ml)[INDEX_LOW]);
	p += MUL32(a0, k0);
	q =  MUL32(a1, k3);
	q += MUL32(a2, k2);
	q += MUL32(a3, k1);
	q += q;
	p += q;
	t2 = (u32)(p);
	p >>= 32;
	p += (u64)(((u32 *)ml)[INDEX_HIGH]);
	p += MUL32(a0, k1);
	p += MUL32(a1, k0);
	q =  MUL32(a2, k3);
	q += MUL32(a3, k2);
	q += q;
	p += q;
	*(u64 *)(alo) = (p << 32) | t2;
	p >>= 32;
	*(u64 *)(ahi) = p + t;

#undef a0
#undef a1
#undef a2
#undef a3
#undef k0
#undef k1
#undef k2
#undef k3
}
