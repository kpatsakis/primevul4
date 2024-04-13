static int tg3_phydsp_read(struct tg3 *tp, u32 reg, u32 *val)
{
	int err;

	err = tg3_writephy(tp, MII_TG3_DSP_ADDRESS, reg);
	if (!err)
		err = tg3_readphy(tp, MII_TG3_DSP_RW_PORT, val);

	return err;
}
