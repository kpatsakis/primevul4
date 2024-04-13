int submit_bio_wait(struct bio *bio)
{
	struct submit_bio_ret ret;

	init_completion(&ret.event);
	bio->bi_private = &ret;
	bio->bi_end_io = submit_bio_wait_endio;
	bio->bi_opf |= REQ_SYNC;
	submit_bio(bio);
	wait_for_completion_io(&ret.event);

	return ret.error;
}
