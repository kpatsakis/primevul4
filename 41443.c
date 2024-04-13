static void tg3_phy_set_wirespeed(struct tg3 *tp)
{
	int ret;
	u32 val;

	if (tp->phy_flags & TG3_PHYFLG_NO_ETH_WIRE_SPEED)
		return;

	ret = tg3_phy_auxctl_read(tp, MII_TG3_AUXCTL_SHDWSEL_MISC, &val);
	if (!ret)
		tg3_phy_auxctl_write(tp, MII_TG3_AUXCTL_SHDWSEL_MISC,
				     val | MII_TG3_AUXCTL_MISC_WIRESPD_EN);
}
