static void check_raid1c34_incompat_flag(struct btrfs_fs_info *info, u64 type)
{
	if (!(type & (BTRFS_BLOCK_GROUP_RAID1C3 | BTRFS_BLOCK_GROUP_RAID1C4)))
		return;

	btrfs_set_fs_incompat(info, RAID1C34);
}