static void wait_for_snapshot_creation(struct btrfs_root *root)
{
	while (true) {
		int ret;

		ret = btrfs_start_write_no_snapshoting(root);
		if (ret)
			break;
		wait_on_atomic_t(&root->will_be_snapshoted,
				 wait_snapshoting_atomic_t,
				 TASK_UNINTERRUPTIBLE);
	}
}
