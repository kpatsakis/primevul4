static void wanxl_reset(card_t *card)
{
	u32 old_value = readl(card->plx + PLX_CONTROL) & ~PLX_CTL_RESET;

	writel(0x80, card->plx + PLX_MAILBOX_0);
	writel(old_value | PLX_CTL_RESET, card->plx + PLX_CONTROL);
	readl(card->plx + PLX_CONTROL); /* wait for posted write */
	udelay(1);
	writel(old_value, card->plx + PLX_CONTROL);
	readl(card->plx + PLX_CONTROL); /* wait for posted write */
}
