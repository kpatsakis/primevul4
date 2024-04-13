static void tg3_write32_mbox_5906(struct tg3 *tp, u32 off, u32 val)
{
	writel(val, tp->regs + off + GRCMBOX_BASE);
}
