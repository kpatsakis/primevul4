static inline int validate_convert_profile(struct btrfs_fs_info *fs_info,
		const struct btrfs_balance_args *bargs,
		u64 allowed, const char *type)
{
	if (!(bargs->flags & BTRFS_BALANCE_ARGS_CONVERT))
		return true;

	if (fs_info->sectorsize < PAGE_SIZE &&
		bargs->target & BTRFS_BLOCK_GROUP_RAID56_MASK) {
		btrfs_err(fs_info,
		"RAID56 is not yet supported for sectorsize %u with page size %lu",
			  fs_info->sectorsize, PAGE_SIZE);
		return false;
	}
	/* Profile is valid and does not have bits outside of the allowed set */
	if (alloc_profile_is_valid(bargs->target, 1) &&
	    (bargs->target & ~allowed) == 0)
		return true;

	btrfs_err(fs_info, "balance: invalid convert %s profile %s",
			type, btrfs_bg_type_to_raid_name(bargs->target));
	return false;
}