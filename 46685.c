int mddev_congested(struct mddev *mddev, int bits)
{
	struct md_personality *pers = mddev->pers;
	int ret = 0;

	rcu_read_lock();
	if (mddev->suspended)
		ret = 1;
	else if (pers && pers->congested)
		ret = pers->congested(mddev, bits);
	rcu_read_unlock();
	return ret;
}
