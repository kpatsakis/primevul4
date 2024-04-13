static void init_alloc_chunk_ctl(struct btrfs_fs_devices *fs_devices,
				 struct alloc_chunk_ctl *ctl)
{
	int index = btrfs_bg_flags_to_raid_index(ctl->type);

	ctl->sub_stripes = btrfs_raid_array[index].sub_stripes;
	ctl->dev_stripes = btrfs_raid_array[index].dev_stripes;
	ctl->devs_max = btrfs_raid_array[index].devs_max;
	if (!ctl->devs_max)
		ctl->devs_max = BTRFS_MAX_DEVS(fs_devices->fs_info);
	ctl->devs_min = btrfs_raid_array[index].devs_min;
	ctl->devs_increment = btrfs_raid_array[index].devs_increment;
	ctl->ncopies = btrfs_raid_array[index].ncopies;
	ctl->nparity = btrfs_raid_array[index].nparity;
	ctl->ndevs = 0;

	switch (fs_devices->chunk_alloc_policy) {
	case BTRFS_CHUNK_ALLOC_REGULAR:
		init_alloc_chunk_ctl_policy_regular(fs_devices, ctl);
		break;
	case BTRFS_CHUNK_ALLOC_ZONED:
		init_alloc_chunk_ctl_policy_zoned(fs_devices, ctl);
		break;
	default:
		BUG();
	}
}