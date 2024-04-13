static void warn_32bit_meta_chunk(struct btrfs_fs_info *fs_info,
				  u64 logical, u64 length, u64 type)
{
	if (!(type & BTRFS_BLOCK_GROUP_METADATA))
		return;

	if (logical + length < BTRFS_32BIT_EARLY_WARN_THRESHOLD)
		return;

	btrfs_warn_32bit_limit(fs_info);
}