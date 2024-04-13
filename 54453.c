static void dev_removed(struct tcmu_device *dev)
{
	struct tcmur_device *rdev = tcmu_get_daemon_dev_private(dev);
	int ret;

	/*
	 * The order of cleaning up worker threads and calling ->removed()
	 * is important: for sync handlers, the worker thread needs to be
	 * terminated before removing the handler (i.e., calling handlers
	 * ->close() callout) in order to ensure that no handler callouts
	 * are getting invoked when shutting down the handler.
	 */
	cleanup_io_work_queue_threads(dev);
	tcmulib_cleanup_cmdproc_thread(dev);

	cleanup_io_work_queue(dev, false);
	cleanup_aio_tracking(rdev);

	ret = pthread_mutex_destroy(&rdev->format_lock);
	if (ret != 0)
		tcmu_err("could not cleanup format lock %d\n", ret);

	ret = pthread_mutex_destroy(&rdev->caw_lock);
	if (ret != 0)
		tcmu_err("could not cleanup caw lock %d\n", ret);

	ret = pthread_spin_destroy(&rdev->lock);
	if (ret != 0)
		tcmu_err("could not cleanup mailbox lock %d\n", ret);

	free(rdev);
}
