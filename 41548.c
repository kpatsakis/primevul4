static void tg3_tx_recover(struct tg3 *tp)
{
	BUG_ON(tg3_flag(tp, MBOX_WRITE_REORDER) ||
	       tp->write32_tx_mbox == tg3_write_indirect_mbox);

	netdev_warn(tp->dev,
		    "The system may be re-ordering memory-mapped I/O "
		    "cycles to the network device, attempting to recover. "
		    "Please report the problem to the driver maintainer "
		    "and include system chipset information.\n");

	spin_lock(&tp->lock);
	tg3_flag_set(tp, TX_RECOVERY_PENDING);
	spin_unlock(&tp->lock);
}
