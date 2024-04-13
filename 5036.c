void btrfs_get_bbio(struct btrfs_bio *bbio)
{
	WARN_ON(!refcount_read(&bbio->refs));
	refcount_inc(&bbio->refs);
}