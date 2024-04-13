void btrfs_put_bbio(struct btrfs_bio *bbio)
{
	if (!bbio)
		return;
	if (refcount_dec_and_test(&bbio->refs))
		kfree(bbio);
}