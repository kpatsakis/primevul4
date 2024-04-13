static u32 tg3_read32_mbox_5906(struct tg3 *tp, u32 off)
{
	return readl(tp->regs + off + GRCMBOX_BASE);
}
