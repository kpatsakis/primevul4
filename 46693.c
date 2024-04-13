void mddev_suspend(struct mddev *mddev)
{
	BUG_ON(mddev->suspended);
	mddev->suspended = 1;
	synchronize_rcu();
	wait_event(mddev->sb_wait, atomic_read(&mddev->active_io) == 0);
	mddev->pers->quiesce(mddev, 1);

	del_timer_sync(&mddev->safemode_timer);
}
