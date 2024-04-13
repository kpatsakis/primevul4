static void end_block_io_op(struct bio *bio, int error)
{
	__end_block_io_op(bio->bi_private, error);
	bio_put(bio);
}
