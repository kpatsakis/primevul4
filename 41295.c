static void tg3_ape_write32(struct tg3 *tp, u32 off, u32 val)
{
	writel(val, tp->aperegs + off);
}
