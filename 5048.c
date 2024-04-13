int btrfs_map_sblock(struct btrfs_fs_info *fs_info, enum btrfs_map_op op,
		     u64 logical, u64 *length,
		     struct btrfs_bio **bbio_ret)
{
	return __btrfs_map_block(fs_info, op, logical, length, bbio_ret, 0, 1);
}