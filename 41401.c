static void tg3_mdio_start(struct tg3 *tp)
{
	tp->mi_mode &= ~MAC_MI_MODE_AUTO_POLL;
	tw32_f(MAC_MI_MODE, tp->mi_mode);
	udelay(80);

	if (tg3_flag(tp, MDIOBUS_INITED) &&
	    tg3_asic_rev(tp) == ASIC_REV_5785)
		tg3_mdio_config_5785(tp);
}
