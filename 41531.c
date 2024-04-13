static bool tg3_test_and_report_link_chg(struct tg3 *tp, int curr_link_up)
{
	if (curr_link_up != tp->link_up) {
		if (curr_link_up) {
			netif_carrier_on(tp->dev);
		} else {
			netif_carrier_off(tp->dev);
			if (tp->phy_flags & TG3_PHYFLG_MII_SERDES)
				tp->phy_flags &= ~TG3_PHYFLG_PARALLEL_DETECT;
		}

		tg3_link_report(tp);
		return true;
	}

	return false;
}
