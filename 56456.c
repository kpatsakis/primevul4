int bio_associate_current(struct bio *bio)
{
	struct io_context *ioc;

	if (bio->bi_css)
		return -EBUSY;

	ioc = current->io_context;
	if (!ioc)
		return -ENOENT;

	get_io_context_active(ioc);
	bio->bi_ioc = ioc;
	bio->bi_css = task_get_css(current, io_cgrp_id);
	return 0;
}
