static inline struct mddev *mddev_get(struct mddev *mddev)
{
	atomic_inc(&mddev->active);
	return mddev;
}
