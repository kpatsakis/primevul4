struct md_rdev *md_find_rdev_nr_rcu(struct mddev *mddev, int nr)
{
	struct md_rdev *rdev;

	rdev_for_each_rcu(rdev, mddev)
		if (rdev->desc_nr == nr)
			return rdev;

	return NULL;
}
