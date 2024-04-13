static int __f2fs_commit_super(struct buffer_head *bh,
			struct f2fs_super_block *super)
{
	lock_buffer(bh);
	if (super)
		memcpy(bh->b_data + F2FS_SUPER_OFFSET, super, sizeof(*super));
	set_buffer_uptodate(bh);
	set_buffer_dirty(bh);
	unlock_buffer(bh);

	/* it's rare case, we can do fua all the time */
	return __sync_dirty_buffer(bh, REQ_PREFLUSH | REQ_FUA);
}
