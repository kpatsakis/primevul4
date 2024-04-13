static int match_mddev_units(struct mddev *mddev1, struct mddev *mddev2)
{
	struct md_rdev *rdev, *rdev2;

	rcu_read_lock();
	rdev_for_each_rcu(rdev, mddev1)
		rdev_for_each_rcu(rdev2, mddev2)
			if (rdev->bdev->bd_contains ==
			    rdev2->bdev->bd_contains) {
				rcu_read_unlock();
				return 1;
			}
	rcu_read_unlock();
	return 0;
}
