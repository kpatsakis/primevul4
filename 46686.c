static void mddev_delayed_delete(struct work_struct *ws)
{
	struct mddev *mddev = container_of(ws, struct mddev, del_work);

	sysfs_remove_group(&mddev->kobj, &md_bitmap_group);
	kobject_del(&mddev->kobj);
	kobject_put(&mddev->kobj);
}
