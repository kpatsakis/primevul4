static int tg3_readphy(struct tg3 *tp, int reg, u32 *val)
{
	return __tg3_readphy(tp, tp->phy_addr, reg, val);
}
