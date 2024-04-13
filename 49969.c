static bool bsg_complete(struct bsg_device *bd)
{
	bool ret = false;
	bool spin;

	do {
		spin_lock_irq(&bd->lock);

		BUG_ON(bd->done_cmds > bd->queued_cmds);

		/*
		 * All commands consumed.
		 */
		if (bd->done_cmds == bd->queued_cmds)
			ret = true;

		spin = !test_bit(BSG_F_BLOCK, &bd->flags);

		spin_unlock_irq(&bd->lock);
	} while (!ret && spin);

	return ret;
}
