static int relink_is_mergable(struct extent_buffer *leaf,
			      struct btrfs_file_extent_item *fi,
			      struct new_sa_defrag_extent *new)
{
	if (btrfs_file_extent_disk_bytenr(leaf, fi) != new->bytenr)
		return 0;

	if (btrfs_file_extent_type(leaf, fi) != BTRFS_FILE_EXTENT_REG)
		return 0;

	if (btrfs_file_extent_compression(leaf, fi) != new->compress_type)
		return 0;

	if (btrfs_file_extent_encryption(leaf, fi) ||
	    btrfs_file_extent_other_encoding(leaf, fi))
		return 0;

	return 1;
}
