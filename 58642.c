static inline void end_dir_add(struct inode *dir, unsigned n)
{
	smp_store_release(&dir->i_dir_seq, n + 2);
}
