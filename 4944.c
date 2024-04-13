static u64 calc_data_stripes(u64 type, int num_stripes)
{
	const int index = btrfs_bg_flags_to_raid_index(type);
	const int ncopies = btrfs_raid_array[index].ncopies;
	const int nparity = btrfs_raid_array[index].nparity;

	return (num_stripes - nparity) / ncopies;
}