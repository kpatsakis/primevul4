void md_stop_writes(struct mddev *mddev)
{
	mddev_lock_nointr(mddev);
	__md_stop_writes(mddev);
	mddev_unlock(mddev);
}
