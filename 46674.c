static void md_submit_flush_data(struct work_struct *ws)
{
	struct mddev *mddev = container_of(ws, struct mddev, flush_work);
	struct bio *bio = mddev->flush_bio;

	if (bio->bi_iter.bi_size == 0)
		/* an empty barrier - all done */
		bio_endio(bio, 0);
	else {
		bio->bi_rw &= ~REQ_FLUSH;
		mddev->pers->make_request(mddev, bio);
	}

	mddev->flush_bio = NULL;
	wake_up(&mddev->sb_wait);
}
