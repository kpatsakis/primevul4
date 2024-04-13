int dgnc_mgmt_close(struct inode *inode, struct file *file)
{
	unsigned long flags;
	unsigned int minor = iminor(inode);

	spin_lock_irqsave(&dgnc_global_lock, flags);

	/* mgmt device */
	if (minor < MAXMGMTDEVICES) {
		if (dgnc_mgmt_in_use[minor])
			dgnc_mgmt_in_use[minor] = 0;
	}
	spin_unlock_irqrestore(&dgnc_global_lock, flags);

	return 0;
}
