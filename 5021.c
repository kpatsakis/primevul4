int btrfs_bg_type_to_factor(u64 flags)
{
	const int index = btrfs_bg_flags_to_raid_index(flags);

	return btrfs_raid_array[index].ncopies;
}