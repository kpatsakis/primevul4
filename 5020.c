static bool need_full_stripe(enum btrfs_map_op op)
{
	return (op == BTRFS_MAP_WRITE || op == BTRFS_MAP_GET_READ_MIRRORS);
}