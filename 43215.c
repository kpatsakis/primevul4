static int isofs_fill_super(struct super_block *s, void *data, int silent)
{
	struct buffer_head *bh = NULL, *pri_bh = NULL;
	struct hs_primary_descriptor *h_pri = NULL;
	struct iso_primary_descriptor *pri = NULL;
	struct iso_supplementary_descriptor *sec = NULL;
	struct iso_directory_record *rootp;
	struct inode *inode;
	struct iso9660_options opt;
	struct isofs_sb_info *sbi;
	unsigned long first_data_zone;
	int joliet_level = 0;
	int iso_blknum, block;
	int orig_zonesize;
	int table, error = -EINVAL;
	unsigned int vol_desc_start;

	save_mount_options(s, data);

	sbi = kzalloc(sizeof(*sbi), GFP_KERNEL);
	if (!sbi)
		return -ENOMEM;
	s->s_fs_info = sbi;

	if (!parse_options((char *)data, &opt))
		goto out_freesbi;

	/*
	 * First of all, get the hardware blocksize for this device.
	 * If we don't know what it is, or the hardware blocksize is
	 * larger than the blocksize the user specified, then use
	 * that value.
	 */
	/*
	 * What if bugger tells us to go beyond page size?
	 */
	opt.blocksize = sb_min_blocksize(s, opt.blocksize);

	sbi->s_high_sierra = 0; /* default is iso9660 */

	vol_desc_start = (opt.sbsector != -1) ?
		opt.sbsector : isofs_get_last_session(s,opt.session);

	for (iso_blknum = vol_desc_start+16;
		iso_blknum < vol_desc_start+100; iso_blknum++) {
		struct hs_volume_descriptor *hdp;
		struct iso_volume_descriptor  *vdp;

		block = iso_blknum << (ISOFS_BLOCK_BITS - s->s_blocksize_bits);
		if (!(bh = sb_bread(s, block)))
			goto out_no_read;

		vdp = (struct iso_volume_descriptor *)bh->b_data;
		hdp = (struct hs_volume_descriptor *)bh->b_data;

		/*
		 * Due to the overlapping physical location of the descriptors,
		 * ISO CDs can match hdp->id==HS_STANDARD_ID as well. To ensure
		 * proper identification in this case, we first check for ISO.
		 */
		if (strncmp (vdp->id, ISO_STANDARD_ID, sizeof vdp->id) == 0) {
			if (isonum_711(vdp->type) == ISO_VD_END)
				break;
			if (isonum_711(vdp->type) == ISO_VD_PRIMARY) {
				if (pri == NULL) {
					pri = (struct iso_primary_descriptor *)vdp;
					/* Save the buffer in case we need it ... */
					pri_bh = bh;
					bh = NULL;
				}
			}
#ifdef CONFIG_JOLIET
			else if (isonum_711(vdp->type) == ISO_VD_SUPPLEMENTARY) {
				sec = (struct iso_supplementary_descriptor *)vdp;
				if (sec->escape[0] == 0x25 && sec->escape[1] == 0x2f) {
					if (opt.joliet) {
						if (sec->escape[2] == 0x40)
							joliet_level = 1;
						else if (sec->escape[2] == 0x43)
							joliet_level = 2;
						else if (sec->escape[2] == 0x45)
							joliet_level = 3;

						printk(KERN_DEBUG "ISO 9660 Extensions: "
							"Microsoft Joliet Level %d\n",
							joliet_level);
					}
					goto root_found;
				} else {
				/* Unknown supplementary volume descriptor */
				sec = NULL;
				}
			}
#endif
		} else {
			if (strncmp (hdp->id, HS_STANDARD_ID, sizeof hdp->id) == 0) {
				if (isonum_711(hdp->type) != ISO_VD_PRIMARY)
					goto out_freebh;

				sbi->s_high_sierra = 1;
				opt.rock = 0;
				h_pri = (struct hs_primary_descriptor *)vdp;
				goto root_found;
			}
		}

		/* Just skip any volume descriptors we don't recognize */

		brelse(bh);
		bh = NULL;
	}
	/*
	 * If we fall through, either no volume descriptor was found,
	 * or else we passed a primary descriptor looking for others.
	 */
	if (!pri)
		goto out_unknown_format;
	brelse(bh);
	bh = pri_bh;
	pri_bh = NULL;

root_found:

	if (joliet_level && (pri == NULL || !opt.rock)) {
		/* This is the case of Joliet with the norock mount flag.
		 * A disc with both Joliet and Rock Ridge is handled later
		 */
		pri = (struct iso_primary_descriptor *) sec;
	}

	if(sbi->s_high_sierra){
		rootp = (struct iso_directory_record *) h_pri->root_directory_record;
		sbi->s_nzones = isonum_733(h_pri->volume_space_size);
		sbi->s_log_zone_size = isonum_723(h_pri->logical_block_size);
		sbi->s_max_size = isonum_733(h_pri->volume_space_size);
	} else {
		if (!pri)
			goto out_freebh;
		rootp = (struct iso_directory_record *) pri->root_directory_record;
		sbi->s_nzones = isonum_733(pri->volume_space_size);
		sbi->s_log_zone_size = isonum_723(pri->logical_block_size);
		sbi->s_max_size = isonum_733(pri->volume_space_size);
	}

	sbi->s_ninodes = 0; /* No way to figure this out easily */

	orig_zonesize = sbi->s_log_zone_size;
	/*
	 * If the zone size is smaller than the hardware sector size,
	 * this is a fatal error.  This would occur if the disc drive
	 * had sectors that were 2048 bytes, but the filesystem had
	 * blocks that were 512 bytes (which should only very rarely
	 * happen.)
	 */
	if (orig_zonesize < opt.blocksize)
		goto out_bad_size;

	/* RDE: convert log zone size to bit shift */
	switch (sbi->s_log_zone_size) {
	case  512: sbi->s_log_zone_size =  9; break;
	case 1024: sbi->s_log_zone_size = 10; break;
	case 2048: sbi->s_log_zone_size = 11; break;

	default:
		goto out_bad_zone_size;
	}

	s->s_magic = ISOFS_SUPER_MAGIC;

	/*
	 * With multi-extent files, file size is only limited by the maximum
	 * size of a file system, which is 8 TB.
	 */
	s->s_maxbytes = 0x80000000000LL;

	/* Set this for reference. Its not currently used except on write
	   which we don't have .. */

	first_data_zone = isonum_733(rootp->extent) +
			  isonum_711(rootp->ext_attr_length);
	sbi->s_firstdatazone = first_data_zone;
#ifndef BEQUIET
	printk(KERN_DEBUG "ISOFS: Max size:%ld   Log zone size:%ld\n",
		sbi->s_max_size, 1UL << sbi->s_log_zone_size);
	printk(KERN_DEBUG "ISOFS: First datazone:%ld\n", sbi->s_firstdatazone);
	if(sbi->s_high_sierra)
		printk(KERN_DEBUG "ISOFS: Disc in High Sierra format.\n");
#endif

	/*
	 * If the Joliet level is set, we _may_ decide to use the
	 * secondary descriptor, but can't be sure until after we
	 * read the root inode. But before reading the root inode
	 * we may need to change the device blocksize, and would
	 * rather release the old buffer first. So, we cache the
	 * first_data_zone value from the secondary descriptor.
	 */
	if (joliet_level) {
		pri = (struct iso_primary_descriptor *) sec;
		rootp = (struct iso_directory_record *)
			pri->root_directory_record;
		first_data_zone = isonum_733(rootp->extent) +
				isonum_711(rootp->ext_attr_length);
	}

	/*
	 * We're all done using the volume descriptor, and may need
	 * to change the device blocksize, so release the buffer now.
	 */
	brelse(pri_bh);
	brelse(bh);

	/*
	 * Force the blocksize to 512 for 512 byte sectors.  The file
	 * read primitives really get it wrong in a bad way if we don't
	 * do this.
	 *
	 * Note - we should never be setting the blocksize to something
	 * less than the hardware sector size for the device.  If we
	 * do, we would end up having to read larger buffers and split
	 * out portions to satisfy requests.
	 *
	 * Note2- the idea here is that we want to deal with the optimal
	 * zonesize in the filesystem.  If we have it set to something less,
	 * then we have horrible problems with trying to piece together
	 * bits of adjacent blocks in order to properly read directory
	 * entries.  By forcing the blocksize in this way, we ensure
	 * that we will never be required to do this.
	 */
	sb_set_blocksize(s, orig_zonesize);

	sbi->s_nls_iocharset = NULL;

#ifdef CONFIG_JOLIET
	if (joliet_level && opt.utf8 == 0) {
		char *p = opt.iocharset ? opt.iocharset : CONFIG_NLS_DEFAULT;
		sbi->s_nls_iocharset = load_nls(p);
		if (! sbi->s_nls_iocharset) {
			/* Fail only if explicit charset specified */
			if (opt.iocharset)
				goto out_freesbi;
			sbi->s_nls_iocharset = load_nls_default();
		}
	}
#endif
	s->s_op = &isofs_sops;
	s->s_export_op = &isofs_export_ops;
	sbi->s_mapping = opt.map;
	sbi->s_rock = (opt.rock ? 2 : 0);
	sbi->s_rock_offset = -1; /* initial offset, will guess until SP is found*/
	sbi->s_cruft = opt.cruft;
	sbi->s_hide = opt.hide;
	sbi->s_showassoc = opt.showassoc;
	sbi->s_uid = opt.uid;
	sbi->s_gid = opt.gid;
	sbi->s_uid_set = opt.uid_set;
	sbi->s_gid_set = opt.gid_set;
	sbi->s_utf8 = opt.utf8;
	sbi->s_nocompress = opt.nocompress;
	sbi->s_overriderockperm = opt.overriderockperm;
	/*
	 * It would be incredibly stupid to allow people to mark every file
	 * on the disk as suid, so we merely allow them to set the default
	 * permissions.
	 */
	if (opt.fmode != ISOFS_INVALID_MODE)
		sbi->s_fmode = opt.fmode & 0777;
	else
		sbi->s_fmode = ISOFS_INVALID_MODE;
	if (opt.dmode != ISOFS_INVALID_MODE)
		sbi->s_dmode = opt.dmode & 0777;
	else
		sbi->s_dmode = ISOFS_INVALID_MODE;

	/*
	 * Read the root inode, which _may_ result in changing
	 * the s_rock flag. Once we have the final s_rock value,
	 * we then decide whether to use the Joliet descriptor.
	 */
	inode = isofs_iget(s, sbi->s_firstdatazone, 0);
	if (IS_ERR(inode))
		goto out_no_root;

	/*
	 * Fix for broken CDs with Rock Ridge and empty ISO root directory but
	 * correct Joliet root directory.
	 */
	if (sbi->s_rock == 1 && joliet_level &&
				rootdir_empty(s, sbi->s_firstdatazone)) {
		printk(KERN_NOTICE
			"ISOFS: primary root directory is empty. "
			"Disabling Rock Ridge and switching to Joliet.");
		sbi->s_rock = 0;
	}

	/*
	 * If this disk has both Rock Ridge and Joliet on it, then we
	 * want to use Rock Ridge by default.  This can be overridden
	 * by using the norock mount option.  There is still one other
	 * possibility that is not taken into account: a Rock Ridge
	 * CD with Unicode names.  Until someone sees such a beast, it
	 * will not be supported.
	 */
	if (sbi->s_rock == 1) {
		joliet_level = 0;
	} else if (joliet_level) {
		sbi->s_rock = 0;
		if (sbi->s_firstdatazone != first_data_zone) {
			sbi->s_firstdatazone = first_data_zone;
			printk(KERN_DEBUG
				"ISOFS: changing to secondary root\n");
			iput(inode);
			inode = isofs_iget(s, sbi->s_firstdatazone, 0);
			if (IS_ERR(inode))
				goto out_no_root;
		}
	}

	if (opt.check == 'u') {
		/* Only Joliet is case insensitive by default */
		if (joliet_level)
			opt.check = 'r';
		else
			opt.check = 's';
	}
	sbi->s_joliet_level = joliet_level;

	/* Make sure the root inode is a directory */
	if (!S_ISDIR(inode->i_mode)) {
		printk(KERN_WARNING
			"isofs_fill_super: root inode is not a directory. "
			"Corrupted media?\n");
		goto out_iput;
	}

	table = 0;
	if (joliet_level)
		table += 2;
	if (opt.check == 'r')
		table++;

	s->s_d_op = &isofs_dentry_ops[table];

	/* get the root dentry */
	s->s_root = d_make_root(inode);
	if (!(s->s_root)) {
		error = -ENOMEM;
		goto out_no_inode;
	}

	kfree(opt.iocharset);

	return 0;

	/*
	 * Display error messages and free resources.
	 */
out_iput:
	iput(inode);
	goto out_no_inode;
out_no_root:
	error = PTR_ERR(inode);
	if (error != -ENOMEM)
		printk(KERN_WARNING "%s: get root inode failed\n", __func__);
out_no_inode:
#ifdef CONFIG_JOLIET
	unload_nls(sbi->s_nls_iocharset);
#endif
	goto out_freesbi;
out_no_read:
	printk(KERN_WARNING "%s: bread failed, dev=%s, iso_blknum=%d, block=%d\n",
		__func__, s->s_id, iso_blknum, block);
	goto out_freebh;
out_bad_zone_size:
	printk(KERN_WARNING "ISOFS: Bad logical zone size %ld\n",
		sbi->s_log_zone_size);
	goto out_freebh;
out_bad_size:
	printk(KERN_WARNING "ISOFS: Logical zone size(%d) < hardware blocksize(%u)\n",
		orig_zonesize, opt.blocksize);
	goto out_freebh;
out_unknown_format:
	if (!silent)
		printk(KERN_WARNING "ISOFS: Unable to identify CD-ROM format.\n");

out_freebh:
	brelse(bh);
	brelse(pri_bh);
out_freesbi:
	kfree(opt.iocharset);
	kfree(sbi);
	s->s_fs_info = NULL;
	return error;
}
