static inline void tg3_rd32_loop(struct tg3 *tp, u32 *dst, u32 off, u32 len)
{
	int i;

	dst = (u32 *)((u8 *)dst + off);
	for (i = 0; i < len; i += sizeof(u32))
		*dst++ = tr32(off + i);
}
