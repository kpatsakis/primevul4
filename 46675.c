void md_super_wait(struct mddev *mddev)
{
	/* wait for all superblock writes that were scheduled to complete */
	wait_event(mddev->sb_wait, atomic_read(&mddev->pending_writes)==0);
}
