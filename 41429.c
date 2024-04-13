static int tg3_phy_auxctl_write(struct tg3 *tp, int reg, u32 set)
{
	if (reg == MII_TG3_AUXCTL_SHDWSEL_MISC)
		set |= MII_TG3_AUXCTL_MISC_WREN;

	return tg3_writephy(tp, MII_TG3_AUX_CTRL, set | reg);
}
