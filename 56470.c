void bio_disassociate_task(struct bio *bio)
{
	if (bio->bi_ioc) {
		put_io_context(bio->bi_ioc);
		bio->bi_ioc = NULL;
	}
	if (bio->bi_css) {
		css_put(bio->bi_css);
		bio->bi_css = NULL;
	}
}
