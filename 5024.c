static void btrfs_report_missing_device(struct btrfs_fs_info *fs_info,
					u64 devid, u8 *uuid, bool error)
{
	if (error)
		btrfs_err_rl(fs_info, "devid %llu uuid %pU is missing",
			      devid, uuid);
	else
		btrfs_warn_rl(fs_info, "devid %llu uuid %pU is missing",
			      devid, uuid);
}