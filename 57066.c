static int init_percpu_info(struct f2fs_sb_info *sbi)
{
	int err;

	err = percpu_counter_init(&sbi->alloc_valid_block_count, 0, GFP_KERNEL);
	if (err)
		return err;

	return percpu_counter_init(&sbi->total_valid_inode_count, 0,
								GFP_KERNEL);
}
