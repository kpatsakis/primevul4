struct bio *bio_clone_mddev(struct bio *bio, gfp_t gfp_mask,
			    struct mddev *mddev)
{
	if (!mddev || !mddev->bio_set)
		return bio_clone(bio, gfp_mask);

	return bio_clone_bioset(bio, gfp_mask, mddev->bio_set);
}
