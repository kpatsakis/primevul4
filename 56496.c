static void submit_bio_wait_endio(struct bio *bio)
{
	struct submit_bio_ret *ret = bio->bi_private;

	ret->error = blk_status_to_errno(bio->bi_status);
	complete(&ret->event);
}
