int dgnc_mgmt_open(struct inode *inode, struct file *file)
{
	unsigned long flags;
	unsigned int minor = iminor(inode);

	spin_lock_irqsave(&dgnc_global_lock, flags);

	/* mgmt device */
	if (minor < MAXMGMTDEVICES) {
		/* Only allow 1 open at a time on mgmt device */
		if (dgnc_mgmt_in_use[minor]) {
			spin_unlock_irqrestore(&dgnc_global_lock, flags);
			return -EBUSY;
		}
		dgnc_mgmt_in_use[minor]++;
	} else {
		spin_unlock_irqrestore(&dgnc_global_lock, flags);
		return -ENXIO;
	}

	spin_unlock_irqrestore(&dgnc_global_lock, flags);

	return 0;
}
