static __exit void md_exit(void)
{
	struct mddev *mddev;
	struct list_head *tmp;
	int delay = 1;

	blk_unregister_region(MKDEV(MD_MAJOR,0), 512);
	blk_unregister_region(MKDEV(mdp_major,0), 1U << MINORBITS);

	unregister_blkdev(MD_MAJOR,"md");
	unregister_blkdev(mdp_major, "mdp");
	unregister_reboot_notifier(&md_notifier);
	unregister_sysctl_table(raid_table_header);

	/* We cannot unload the modules while some process is
	 * waiting for us in select() or poll() - wake them up
	 */
	md_unloading = 1;
	while (waitqueue_active(&md_event_waiters)) {
		/* not safe to leave yet */
		wake_up(&md_event_waiters);
		msleep(delay);
		delay += delay;
	}
	remove_proc_entry("mdstat", NULL);

	for_each_mddev(mddev, tmp) {
		export_array(mddev);
		mddev->hold_active = 0;
	}
	destroy_workqueue(md_misc_wq);
	destroy_workqueue(md_wq);
}
