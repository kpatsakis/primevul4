static void tg3_write32_tx_mbox(struct tg3 *tp, u32 off, u32 val)
{
	void __iomem *mbox = tp->regs + off;
	writel(val, mbox);
	if (tg3_flag(tp, TXD_MBOX_HWBUG))
		writel(val, mbox);
	if (tg3_flag(tp, MBOX_WRITE_REORDER) ||
	    tg3_flag(tp, FLUSH_POSTED_WRITES))
		readl(mbox);
}
