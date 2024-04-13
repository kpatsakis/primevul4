static void md_delayed_delete(struct work_struct *ws)
{
	struct md_rdev *rdev = container_of(ws, struct md_rdev, del_work);
	kobject_del(&rdev->kobj);
	kobject_put(&rdev->kobj);
}
