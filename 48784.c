static void bdev_inode_switch_bdi(struct inode *inode,
			struct backing_dev_info *dst)
{
	struct backing_dev_info *old = inode->i_data.backing_dev_info;
	bool wakeup_bdi = false;

	if (unlikely(dst == old))		/* deadlock avoidance */
		return;
	bdi_lock_two(&old->wb, &dst->wb);
	spin_lock(&inode->i_lock);
	inode->i_data.backing_dev_info = dst;
	if (inode->i_state & I_DIRTY) {
		if (bdi_cap_writeback_dirty(dst) && !wb_has_dirty_io(&dst->wb))
			wakeup_bdi = true;
		list_move(&inode->i_wb_list, &dst->wb.b_dirty);
	}
	spin_unlock(&inode->i_lock);
	spin_unlock(&old->wb.list_lock);
	spin_unlock(&dst->wb.list_lock);

	if (wakeup_bdi)
		bdi_wakeup_thread_delayed(dst);
}
