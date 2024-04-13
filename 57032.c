static void destroy_percpu_info(struct f2fs_sb_info *sbi)
{
	percpu_counter_destroy(&sbi->alloc_valid_block_count);
	percpu_counter_destroy(&sbi->total_valid_inode_count);
}
