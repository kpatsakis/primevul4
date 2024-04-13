static int bd_prepare_to_claim(struct block_device *bdev,
			       struct block_device *whole, void *holder)
{
retry:
	/* if someone else claimed, fail */
	if (!bd_may_claim(bdev, whole, holder))
		return -EBUSY;

	/* if claiming is already in progress, wait for it to finish */
	if (whole->bd_claiming) {
		wait_queue_head_t *wq = bit_waitqueue(&whole->bd_claiming, 0);
		DEFINE_WAIT(wait);

		prepare_to_wait(wq, &wait, TASK_UNINTERRUPTIBLE);
		spin_unlock(&bdev_lock);
		schedule();
		finish_wait(wq, &wait);
		spin_lock(&bdev_lock);
		goto retry;
	}

	/* yay, all mine */
	return 0;
}
