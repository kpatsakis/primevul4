void md_stop(struct mddev *mddev)
{
	/* stop the array and free an attached data structures.
	 * This is called from dm-raid
	 */
	__md_stop(mddev);
	bitmap_destroy(mddev);
	if (mddev->bio_set)
		bioset_free(mddev->bio_set);
}
