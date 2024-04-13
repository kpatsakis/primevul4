static void autorun_array(struct mddev *mddev)
{
	struct md_rdev *rdev;
	int err;

	if (list_empty(&mddev->disks))
		return;

	printk(KERN_INFO "md: running: ");

	rdev_for_each(rdev, mddev) {
		char b[BDEVNAME_SIZE];
		printk("<%s>", bdevname(rdev->bdev,b));
	}
	printk("\n");

	err = do_md_run(mddev);
	if (err) {
		printk(KERN_WARNING "md: do_md_run() returned %d\n", err);
		do_md_stop(mddev, 0, NULL);
	}
}
