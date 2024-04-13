static void tg3_write_flush_reg32(struct tg3 *tp, u32 off, u32 val)
{
	writel(val, tp->regs + off);
	readl(tp->regs + off);
}
