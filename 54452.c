static int dev_added(struct tcmu_device *dev)
{
	struct tcmur_handler *rhandler = tcmu_get_runner_handler(dev);
	struct tcmur_device *rdev;
	int32_t block_size, max_sectors;
	int64_t dev_size;
	int ret;

	rdev = calloc(1, sizeof(*rdev));
	if (!rdev)
		return -ENOMEM;
	tcmu_set_daemon_dev_private(dev, rdev);

	ret = -EINVAL;
	block_size = tcmu_get_attribute(dev, "hw_block_size");
	if (block_size <= 0) {
		tcmu_dev_err(dev, "Could not get hw_block_size\n");
		goto free_rdev;
	}
	tcmu_set_dev_block_size(dev, block_size);

	dev_size = tcmu_get_device_size(dev);
	if (dev_size < 0) {
		tcmu_dev_err(dev, "Could not get device size\n");
		goto free_rdev;
	}
	tcmu_set_dev_num_lbas(dev, dev_size / block_size);

	max_sectors = tcmu_get_attribute(dev, "hw_max_sectors");
	if (max_sectors < 0)
		goto free_rdev;
	tcmu_set_dev_max_xfer_len(dev, max_sectors);

	tcmu_dev_dbg(dev, "Got block_size %ld, size in bytes %lld\n",
		     block_size, dev_size);

	ret = pthread_spin_init(&rdev->lock, 0);
	if (ret != 0)
		goto free_rdev;

	ret = pthread_mutex_init(&rdev->caw_lock, NULL);
	if (ret != 0)
		goto cleanup_dev_lock;

	ret = pthread_mutex_init(&rdev->format_lock, NULL);
	if (ret != 0)
		goto cleanup_caw_lock;

	ret = setup_io_work_queue(dev);
	if (ret < 0)
		goto cleanup_format_lock;

	ret = setup_aio_tracking(rdev);
	if (ret < 0)
		goto cleanup_io_work_queue;

	ret = rhandler->open(dev);
	if (ret)
		goto cleanup_aio_tracking;

	ret = tcmulib_start_cmdproc_thread(dev, tcmur_cmdproc_thread);
	if (ret < 0)
		goto close_dev;

	return 0;

close_dev:
	rhandler->close(dev);
cleanup_aio_tracking:
	cleanup_aio_tracking(rdev);
cleanup_io_work_queue:
	cleanup_io_work_queue(dev, true);
cleanup_format_lock:
	pthread_mutex_destroy(&rdev->format_lock);
cleanup_caw_lock:
	pthread_mutex_destroy(&rdev->caw_lock);
cleanup_dev_lock:
	pthread_spin_destroy(&rdev->lock);
free_rdev:
	free(rdev);
	return ret;
}
