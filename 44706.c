static void __exit floppy_module_exit(void)
{
	int drive;

	blk_unregister_region(MKDEV(FLOPPY_MAJOR, 0), 256);
	unregister_blkdev(FLOPPY_MAJOR, "fd");
	platform_driver_unregister(&floppy_driver);

	destroy_workqueue(floppy_wq);

	for (drive = 0; drive < N_DRIVE; drive++) {
		del_timer_sync(&motor_off_timer[drive]);

		if (floppy_available(drive)) {
			del_gendisk(disks[drive]);
			device_remove_file(&floppy_device[drive].dev, &dev_attr_cmos);
			platform_device_unregister(&floppy_device[drive]);
		}
		blk_cleanup_queue(disks[drive]->queue);

		/*
		 * These disks have not called add_disk().  Don't put down
		 * queue reference in put_disk().
		 */
		if (!(allowed_drive_mask & (1 << drive)) ||
		    fdc_state[FDC(drive)].version == FDC_NONE)
			disks[drive]->queue = NULL;

		put_disk(disks[drive]);
	}

	cancel_delayed_work_sync(&fd_timeout);
	cancel_delayed_work_sync(&fd_timer);

	if (atomic_read(&usage_count))
		floppy_release_irq_and_dma();

	/* eject disk, if any */
	fd_eject(0);
}
