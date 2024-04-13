static void check_raid56_incompat_flag(struct btrfs_fs_info *info, u64 type)
{
	if (!(type & BTRFS_BLOCK_GROUP_RAID56_MASK))
		return;

	btrfs_set_fs_incompat(info, RAID56);
}