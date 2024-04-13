static int check_32bit_meta_chunk(struct btrfs_fs_info *fs_info,
				  u64 logical, u64 length, u64 type)
{
	if (!(type & BTRFS_BLOCK_GROUP_METADATA))
		return 0;

	if (logical + length < MAX_LFS_FILESIZE)
		return 0;

	btrfs_err_32bit_limit(fs_info);
	return -EOVERFLOW;
}