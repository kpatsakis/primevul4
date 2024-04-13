static int set_disk_faulty(struct mddev *mddev, dev_t dev)
{
	struct md_rdev *rdev;
	int err = 0;

	if (mddev->pers == NULL)
		return -ENODEV;

	rcu_read_lock();
	rdev = find_rdev_rcu(mddev, dev);
	if (!rdev)
		err =  -ENODEV;
	else {
		md_error(mddev, rdev);
		if (!test_bit(Faulty, &rdev->flags))
			err = -EBUSY;
	}
	rcu_read_unlock();
	return err;
}
