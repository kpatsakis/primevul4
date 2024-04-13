static int tg3_phy_auxctl_read(struct tg3 *tp, int reg, u32 *val)
{
	int err;

	err = tg3_writephy(tp, MII_TG3_AUX_CTRL,
			   (reg << MII_TG3_AUXCTL_MISC_RDSEL_SHIFT) |
			   MII_TG3_AUXCTL_SHDWSEL_MISC);
	if (!err)
		err = tg3_readphy(tp, MII_TG3_AUX_CTRL, val);

	return err;
}
