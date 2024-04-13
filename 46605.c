static int is_mddev_idle(struct mddev *mddev, int init)
{
	struct md_rdev *rdev;
	int idle;
	int curr_events;

	idle = 1;
	rcu_read_lock();
	rdev_for_each_rcu(rdev, mddev) {
		struct gendisk *disk = rdev->bdev->bd_contains->bd_disk;
		curr_events = (int)part_stat_read(&disk->part0, sectors[0]) +
			      (int)part_stat_read(&disk->part0, sectors[1]) -
			      atomic_read(&disk->sync_io);
		/* sync IO will cause sync_io to increase before the disk_stats
		 * as sync_io is counted when a request starts, and
		 * disk_stats is counted when it completes.
		 * So resync activity will cause curr_events to be smaller than
		 * when there was no such activity.
		 * non-sync IO will cause disk_stat to increase without
		 * increasing sync_io so curr_events will (eventually)
		 * be larger than it was before.  Once it becomes
		 * substantially larger, the test below will cause
		 * the array to appear non-idle, and resync will slow
		 * down.
		 * If there is a lot of outstanding resync activity when
		 * we set last_event to curr_events, then all that activity
		 * completing might cause the array to appear non-idle
		 * and resync will be slowed down even though there might
		 * not have been non-resync activity.  This will only
		 * happen once though.  'last_events' will soon reflect
		 * the state where there is little or no outstanding
		 * resync requests, and further resync activity will
		 * always make curr_events less than last_events.
		 *
		 */
		if (init || curr_events - rdev->last_events > 64) {
			rdev->last_events = curr_events;
			idle = 0;
		}
	}
	rcu_read_unlock();
	return idle;
}
