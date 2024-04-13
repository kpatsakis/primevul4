static void cmdproc_thread_cleanup(void *arg)
{
	struct tcmu_device *dev = arg;
	struct tcmur_handler *rhandler = tcmu_get_runner_handler(dev);

	rhandler->close(dev);
}
