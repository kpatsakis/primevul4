static void schedule_bh(void (*handler)(void))
{
	WARN_ON(work_pending(&floppy_work));

	floppy_work_fn = handler;
	queue_work(floppy_wq, &floppy_work);
}
