static void shmem_recalc_inode(struct inode *inode)
{
	struct shmem_inode_info *info = SHMEM_I(inode);
	long freed;

	freed = info->alloced - info->swapped - inode->i_mapping->nrpages;
	if (freed > 0) {
		struct shmem_sb_info *sbinfo = SHMEM_SB(inode->i_sb);
		if (sbinfo->max_blocks)
			percpu_counter_add(&sbinfo->used_blocks, -freed);
		info->alloced -= freed;
		inode->i_blocks -= freed * BLOCKS_PER_PAGE;
		shmem_unacct_blocks(info->flags, freed);
	}
}
