static void tg3_ape_unlock(struct tg3 *tp, int locknum)
{
	u32 gnt, bit;

	if (!tg3_flag(tp, ENABLE_APE))
		return;

	switch (locknum) {
	case TG3_APE_LOCK_GPIO:
		if (tg3_asic_rev(tp) == ASIC_REV_5761)
			return;
	case TG3_APE_LOCK_GRC:
	case TG3_APE_LOCK_MEM:
		if (!tp->pci_fn)
			bit = APE_LOCK_GRANT_DRIVER;
		else
			bit = 1 << tp->pci_fn;
		break;
	case TG3_APE_LOCK_PHY0:
	case TG3_APE_LOCK_PHY1:
	case TG3_APE_LOCK_PHY2:
	case TG3_APE_LOCK_PHY3:
		bit = APE_LOCK_GRANT_DRIVER;
		break;
	default:
		return;
	}

	if (tg3_asic_rev(tp) == ASIC_REV_5761)
		gnt = TG3_APE_LOCK_GRANT;
	else
		gnt = TG3_APE_PER_LOCK_GRANT;

	tg3_ape_write32(tp, gnt + 4 * locknum, bit);
}
