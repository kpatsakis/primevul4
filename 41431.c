static int tg3_phy_cl45_write(struct tg3 *tp, u32 devad, u32 addr, u32 val)
{
	int err;

	err = tg3_writephy(tp, MII_TG3_MMD_CTRL, devad);
	if (err)
		goto done;

	err = tg3_writephy(tp, MII_TG3_MMD_ADDRESS, addr);
	if (err)
		goto done;

	err = tg3_writephy(tp, MII_TG3_MMD_CTRL,
			   MII_TG3_MMD_CTRL_DATA_NOINC | devad);
	if (err)
		goto done;

	err = tg3_writephy(tp, MII_TG3_MMD_ADDRESS, val);

done:
	return err;
}
