static int __init do_floppy_init(void)
{
	int i, unit, drive, err;

	set_debugt();
	interruptjiffies = resultjiffies = jiffies;

#if defined(CONFIG_PPC)
	if (check_legacy_ioport(FDC1))
		return -ENODEV;
#endif

	raw_cmd = NULL;

	floppy_wq = alloc_ordered_workqueue("floppy", 0);
	if (!floppy_wq)
		return -ENOMEM;

	for (drive = 0; drive < N_DRIVE; drive++) {
		disks[drive] = alloc_disk(1);
		if (!disks[drive]) {
			err = -ENOMEM;
			goto out_put_disk;
		}

		disks[drive]->queue = blk_init_queue(do_fd_request, &floppy_lock);
		if (!disks[drive]->queue) {
			err = -ENOMEM;
			goto out_put_disk;
		}

		blk_queue_max_hw_sectors(disks[drive]->queue, 64);
		disks[drive]->major = FLOPPY_MAJOR;
		disks[drive]->first_minor = TOMINOR(drive);
		disks[drive]->fops = &floppy_fops;
		sprintf(disks[drive]->disk_name, "fd%d", drive);

		init_timer(&motor_off_timer[drive]);
		motor_off_timer[drive].data = drive;
		motor_off_timer[drive].function = motor_off_callback;
	}

	err = register_blkdev(FLOPPY_MAJOR, "fd");
	if (err)
		goto out_put_disk;

	err = platform_driver_register(&floppy_driver);
	if (err)
		goto out_unreg_blkdev;

	blk_register_region(MKDEV(FLOPPY_MAJOR, 0), 256, THIS_MODULE,
			    floppy_find, NULL, NULL);

	for (i = 0; i < 256; i++)
		if (ITYPE(i))
			floppy_sizes[i] = floppy_type[ITYPE(i)].size;
		else
			floppy_sizes[i] = MAX_DISK_SIZE << 1;

	reschedule_timeout(MAXTIMEOUT, "floppy init");
	config_types();

	for (i = 0; i < N_FDC; i++) {
		fdc = i;
		memset(FDCS, 0, sizeof(*FDCS));
		FDCS->dtr = -1;
		FDCS->dor = 0x4;
#if defined(__sparc__) || defined(__mc68000__)
	/*sparcs/sun3x don't have a DOR reset which we can fall back on to */
#ifdef __mc68000__
		if (MACH_IS_SUN3X)
#endif
			FDCS->version = FDC_82072A;
#endif
	}

	use_virtual_dma = can_use_virtual_dma & 1;
	fdc_state[0].address = FDC1;
	if (fdc_state[0].address == -1) {
		cancel_delayed_work(&fd_timeout);
		err = -ENODEV;
		goto out_unreg_region;
	}
#if N_FDC > 1
	fdc_state[1].address = FDC2;
#endif

	fdc = 0;		/* reset fdc in case of unexpected interrupt */
	err = floppy_grab_irq_and_dma();
	if (err) {
		cancel_delayed_work(&fd_timeout);
		err = -EBUSY;
		goto out_unreg_region;
	}

	/* initialise drive state */
	for (drive = 0; drive < N_DRIVE; drive++) {
		memset(UDRS, 0, sizeof(*UDRS));
		memset(UDRWE, 0, sizeof(*UDRWE));
		set_bit(FD_DISK_NEWCHANGE_BIT, &UDRS->flags);
		set_bit(FD_DISK_CHANGED_BIT, &UDRS->flags);
		set_bit(FD_VERIFY_BIT, &UDRS->flags);
		UDRS->fd_device = -1;
		floppy_track_buffer = NULL;
		max_buffer_sectors = 0;
	}
	/*
	 * Small 10 msec delay to let through any interrupt that
	 * initialization might have triggered, to not
	 * confuse detection:
	 */
	msleep(10);

	for (i = 0; i < N_FDC; i++) {
		fdc = i;
		FDCS->driver_version = FD_DRIVER_VERSION;
		for (unit = 0; unit < 4; unit++)
			FDCS->track[unit] = 0;
		if (FDCS->address == -1)
			continue;
		FDCS->rawcmd = 2;
		if (user_reset_fdc(-1, FD_RESET_ALWAYS, false)) {
			/* free ioports reserved by floppy_grab_irq_and_dma() */
			floppy_release_regions(fdc);
			FDCS->address = -1;
			FDCS->version = FDC_NONE;
			continue;
		}
		/* Try to determine the floppy controller type */
		FDCS->version = get_fdc_version();
		if (FDCS->version == FDC_NONE) {
			/* free ioports reserved by floppy_grab_irq_and_dma() */
			floppy_release_regions(fdc);
			FDCS->address = -1;
			continue;
		}
		if (can_use_virtual_dma == 2 && FDCS->version < FDC_82072A)
			can_use_virtual_dma = 0;

		have_no_fdc = 0;
		/* Not all FDCs seem to be able to handle the version command
		 * properly, so force a reset for the standard FDC clones,
		 * to avoid interrupt garbage.
		 */
		user_reset_fdc(-1, FD_RESET_ALWAYS, false);
	}
	fdc = 0;
	cancel_delayed_work(&fd_timeout);
	current_drive = 0;
	initialized = true;
	if (have_no_fdc) {
		DPRINT("no floppy controllers found\n");
		err = have_no_fdc;
		goto out_release_dma;
	}

	for (drive = 0; drive < N_DRIVE; drive++) {
		if (!floppy_available(drive))
			continue;

		floppy_device[drive].name = floppy_device_name;
		floppy_device[drive].id = drive;
		floppy_device[drive].dev.release = floppy_device_release;

		err = platform_device_register(&floppy_device[drive]);
		if (err)
			goto out_remove_drives;

		err = device_create_file(&floppy_device[drive].dev,
					 &dev_attr_cmos);
		if (err)
			goto out_unreg_platform_dev;

		/* to be cleaned up... */
		disks[drive]->private_data = (void *)(long)drive;
		disks[drive]->flags |= GENHD_FL_REMOVABLE;
		disks[drive]->driverfs_dev = &floppy_device[drive].dev;
		add_disk(disks[drive]);
	}

	return 0;

out_unreg_platform_dev:
	platform_device_unregister(&floppy_device[drive]);
out_remove_drives:
	while (drive--) {
		if (floppy_available(drive)) {
			del_gendisk(disks[drive]);
			device_remove_file(&floppy_device[drive].dev, &dev_attr_cmos);
			platform_device_unregister(&floppy_device[drive]);
		}
	}
out_release_dma:
	if (atomic_read(&usage_count))
		floppy_release_irq_and_dma();
out_unreg_region:
	blk_unregister_region(MKDEV(FLOPPY_MAJOR, 0), 256);
	platform_driver_unregister(&floppy_driver);
out_unreg_blkdev:
	unregister_blkdev(FLOPPY_MAJOR, "fd");
out_put_disk:
	destroy_workqueue(floppy_wq);
	for (drive = 0; drive < N_DRIVE; drive++) {
		if (!disks[drive])
			break;
		if (disks[drive]->queue) {
			del_timer_sync(&motor_off_timer[drive]);
			blk_cleanup_queue(disks[drive]->queue);
			disks[drive]->queue = NULL;
		}
		put_disk(disks[drive]);
	}
	return err;
}
