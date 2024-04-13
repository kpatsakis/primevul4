static void md_make_request(struct request_queue *q, struct bio *bio)
{
	const int rw = bio_data_dir(bio);
	struct mddev *mddev = q->queuedata;
	unsigned int sectors;
	int cpu;

	if (mddev == NULL || mddev->pers == NULL
	    || !mddev->ready) {
		bio_io_error(bio);
		return;
	}
	if (mddev->ro == 1 && unlikely(rw == WRITE)) {
		bio_endio(bio, bio_sectors(bio) == 0 ? 0 : -EROFS);
		return;
	}
	smp_rmb(); /* Ensure implications of  'active' are visible */
	rcu_read_lock();
	if (mddev->suspended) {
		DEFINE_WAIT(__wait);
		for (;;) {
			prepare_to_wait(&mddev->sb_wait, &__wait,
					TASK_UNINTERRUPTIBLE);
			if (!mddev->suspended)
				break;
			rcu_read_unlock();
			schedule();
			rcu_read_lock();
		}
		finish_wait(&mddev->sb_wait, &__wait);
	}
	atomic_inc(&mddev->active_io);
	rcu_read_unlock();

	/*
	 * save the sectors now since our bio can
	 * go away inside make_request
	 */
	sectors = bio_sectors(bio);
	mddev->pers->make_request(mddev, bio);

	cpu = part_stat_lock();
	part_stat_inc(cpu, &mddev->gendisk->part0, ios[rw]);
	part_stat_add(cpu, &mddev->gendisk->part0, sectors[rw], sectors);
	part_stat_unlock();

	if (atomic_dec_and_test(&mddev->active_io) && mddev->suspended)
		wake_up(&mddev->sb_wait);
}
