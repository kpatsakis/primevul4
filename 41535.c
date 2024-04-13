static int tg3_test_link(struct tg3 *tp)
{
	int i, max;

	if (!netif_running(tp->dev))
		return -ENODEV;

	if (tp->phy_flags & TG3_PHYFLG_ANY_SERDES)
		max = TG3_SERDES_TIMEOUT_SEC;
	else
		max = TG3_COPPER_TIMEOUT_SEC;

	for (i = 0; i < max; i++) {
		if (tp->link_up)
			return 0;

		if (msleep_interruptible(1000))
			break;
	}

	return -EIO;
}
