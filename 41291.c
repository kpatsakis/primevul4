static void tg3_ape_lock_init(struct tg3 *tp)
{
	int i;
	u32 regbase, bit;

	if (tg3_asic_rev(tp) == ASIC_REV_5761)
		regbase = TG3_APE_LOCK_GRANT;
	else
		regbase = TG3_APE_PER_LOCK_GRANT;

	/* Make sure the driver hasn't any stale locks. */
	for (i = TG3_APE_LOCK_PHY0; i <= TG3_APE_LOCK_GPIO; i++) {
		switch (i) {
		case TG3_APE_LOCK_PHY0:
		case TG3_APE_LOCK_PHY1:
		case TG3_APE_LOCK_PHY2:
		case TG3_APE_LOCK_PHY3:
			bit = APE_LOCK_GRANT_DRIVER;
			break;
		default:
			if (!tp->pci_fn)
				bit = APE_LOCK_GRANT_DRIVER;
			else
				bit = 1 << tp->pci_fn;
		}
		tg3_ape_write32(tp, regbase + 4 * i, bit);
	}

}
