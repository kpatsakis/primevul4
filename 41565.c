static int tg3_writephy(struct tg3 *tp, int reg, u32 val)
{
	return __tg3_writephy(tp, tp->phy_addr, reg, val);
}
