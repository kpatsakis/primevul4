static void catc_stats_timer(unsigned long data)
{
	struct catc *catc = (void *) data;
	int i;

	for (i = 0; i < 8; i++)
		catc_get_reg_async(catc, EthStats + 7 - i, catc_stats_done);

	mod_timer(&catc->timer, jiffies + STATS_UPDATE);
}
