static void md_end_flush(struct bio *bio, int err)
{
	struct md_rdev *rdev = bio->bi_private;
	struct mddev *mddev = rdev->mddev;

	rdev_dec_pending(rdev, mddev);

	if (atomic_dec_and_test(&mddev->flush_pending)) {
		/* The pre-request flush has finished */
		queue_work(md_wq, &mddev->flush_work);
	}
	bio_put(bio);
}
