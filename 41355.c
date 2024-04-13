static void tg3_get_regs(struct net_device *dev,
		struct ethtool_regs *regs, void *_p)
{
	struct tg3 *tp = netdev_priv(dev);

	regs->version = 0;

	memset(_p, 0, TG3_REG_BLK_SIZE);

	if (tp->phy_flags & TG3_PHYFLG_IS_LOW_POWER)
		return;

	tg3_full_lock(tp, 0);

	tg3_dump_legacy_regs(tp, (u32 *)_p);

	tg3_full_unlock(tp);
}
