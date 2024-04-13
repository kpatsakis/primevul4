static void __md_stop(struct mddev *mddev)
{
	struct md_personality *pers = mddev->pers;
	mddev_detach(mddev);
	/* Ensure ->event_work is done */
	flush_workqueue(md_misc_wq);
	spin_lock(&mddev->lock);
	mddev->ready = 0;
	mddev->pers = NULL;
	spin_unlock(&mddev->lock);
	pers->free(mddev, mddev->private);
	mddev->private = NULL;
	if (pers->sync_request && mddev->to_remove == NULL)
		mddev->to_remove = &md_redundancy_group;
	module_put(pers->owner);
	clear_bit(MD_RECOVERY_FROZEN, &mddev->recovery);
}
