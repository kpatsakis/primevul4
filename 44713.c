static void floppy_release_irq_and_dma(void)
{
	int old_fdc;
#ifndef __sparc__
	int drive;
#endif
	long tmpsize;
	unsigned long tmpaddr;

	if (!atomic_dec_and_test(&usage_count))
		return;

	if (irqdma_allocated) {
		fd_disable_dma();
		fd_free_dma();
		fd_free_irq();
		irqdma_allocated = 0;
	}
	set_dor(0, ~0, 8);
#if N_FDC > 1
	set_dor(1, ~8, 0);
#endif

	if (floppy_track_buffer && max_buffer_sectors) {
		tmpsize = max_buffer_sectors * 1024;
		tmpaddr = (unsigned long)floppy_track_buffer;
		floppy_track_buffer = NULL;
		max_buffer_sectors = 0;
		buffer_min = buffer_max = -1;
		fd_dma_mem_free(tmpaddr, tmpsize);
	}
#ifndef __sparc__
	for (drive = 0; drive < N_FDC * 4; drive++)
		if (timer_pending(motor_off_timer + drive))
			pr_info("motor off timer %d still active\n", drive);
#endif

	if (delayed_work_pending(&fd_timeout))
		pr_info("floppy timer still active:%s\n", timeout_message);
	if (delayed_work_pending(&fd_timer))
		pr_info("auxiliary floppy timer still active\n");
	if (work_pending(&floppy_work))
		pr_info("work still pending\n");
	old_fdc = fdc;
	for (fdc = 0; fdc < N_FDC; fdc++)
		if (FDCS->address != -1)
			floppy_release_regions(fdc);
	fdc = old_fdc;
}
