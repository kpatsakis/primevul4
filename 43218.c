static unsigned int isofs_get_last_session(struct super_block *sb, s32 session)
{
	struct cdrom_multisession ms_info;
	unsigned int vol_desc_start;
	struct block_device *bdev = sb->s_bdev;
	int i;

	vol_desc_start=0;
	ms_info.addr_format=CDROM_LBA;
	if(session >= 0 && session <= 99) {
		struct cdrom_tocentry Te;
		Te.cdte_track=session;
		Te.cdte_format=CDROM_LBA;
		i = ioctl_by_bdev(bdev, CDROMREADTOCENTRY, (unsigned long) &Te);
		if (!i) {
			printk(KERN_DEBUG "ISOFS: Session %d start %d type %d\n",
				session, Te.cdte_addr.lba,
				Te.cdte_ctrl&CDROM_DATA_TRACK);
			if ((Te.cdte_ctrl&CDROM_DATA_TRACK) == 4)
				return Te.cdte_addr.lba;
		}

		printk(KERN_ERR "ISOFS: Invalid session number or type of track\n");
	}
	i = ioctl_by_bdev(bdev, CDROMMULTISESSION, (unsigned long) &ms_info);
	if (session > 0)
		printk(KERN_ERR "ISOFS: Invalid session number\n");
#if 0
	printk(KERN_DEBUG "isofs.inode: CDROMMULTISESSION: rc=%d\n",i);
	if (i==0) {
		printk(KERN_DEBUG "isofs.inode: XA disk: %s\n",ms_info.xa_flag?"yes":"no");
		printk(KERN_DEBUG "isofs.inode: vol_desc_start = %d\n", ms_info.addr.lba);
	}
#endif
	if (i==0)
#if WE_OBEY_THE_WRITTEN_STANDARDS
		if (ms_info.xa_flag) /* necessary for a valid ms_info.addr */
#endif
			vol_desc_start=ms_info.addr.lba;
	return vol_desc_start;
}
