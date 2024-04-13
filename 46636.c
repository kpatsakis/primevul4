void md_flush_request(struct mddev *mddev, struct bio *bio)
{
	spin_lock_irq(&mddev->lock);
	wait_event_lock_irq(mddev->sb_wait,
			    !mddev->flush_bio,
			    mddev->lock);
	mddev->flush_bio = bio;
	spin_unlock_irq(&mddev->lock);

	INIT_WORK(&mddev->flush_work, submit_flushes);
	queue_work(md_wq, &mddev->flush_work);
}
