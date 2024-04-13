sync_force_parallel_show(struct mddev *mddev, char *page)
{
	return sprintf(page, "%d\n", mddev->parallel_resync);
}
