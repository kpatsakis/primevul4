static int reiserfs_file_release(struct inode *inode, struct file *filp)
{

	struct reiserfs_transaction_handle th;
	int err;
	int jbegin_failure = 0;

	BUG_ON(!S_ISREG(inode->i_mode));

        if (atomic_add_unless(&REISERFS_I(inode)->openers, -1, 1))
		return 0;

	mutex_lock(&(REISERFS_I(inode)->tailpack));

        if (!atomic_dec_and_test(&REISERFS_I(inode)->openers)) {
		mutex_unlock(&(REISERFS_I(inode)->tailpack));
		return 0;
	}

	/* fast out for when nothing needs to be done */
	if ((!(REISERFS_I(inode)->i_flags & i_pack_on_close_mask) ||
	     !tail_has_to_be_packed(inode)) &&
	    REISERFS_I(inode)->i_prealloc_count <= 0) {
		mutex_unlock(&(REISERFS_I(inode)->tailpack));
		return 0;
	}

	reiserfs_write_lock(inode->i_sb);
	/* freeing preallocation only involves relogging blocks that
	 * are already in the current transaction.  preallocation gets
	 * freed at the end of each transaction, so it is impossible for
	 * us to log any additional blocks (including quota blocks)
	 */
	err = journal_begin(&th, inode->i_sb, 1);
	if (err) {
		/* uh oh, we can't allow the inode to go away while there
		 * is still preallocation blocks pending.  Try to join the
		 * aborted transaction
		 */
		jbegin_failure = err;
		err = journal_join_abort(&th, inode->i_sb, 1);

		if (err) {
			/* hmpf, our choices here aren't good.  We can pin the inode
			 * which will disallow unmount from every happening, we can
			 * do nothing, which will corrupt random memory on unmount,
			 * or we can forcibly remove the file from the preallocation
			 * list, which will leak blocks on disk.  Lets pin the inode
			 * and let the admin know what is going on.
			 */
			igrab(inode);
			reiserfs_warning(inode->i_sb, "clm-9001",
					 "pinning inode %lu because the "
					 "preallocation can't be freed",
					 inode->i_ino);
			goto out;
		}
	}
	reiserfs_update_inode_transaction(inode);

#ifdef REISERFS_PREALLOCATE
	reiserfs_discard_prealloc(&th, inode);
#endif
	err = journal_end(&th, inode->i_sb, 1);

	/* copy back the error code from journal_begin */
	if (!err)
		err = jbegin_failure;

	if (!err &&
	    (REISERFS_I(inode)->i_flags & i_pack_on_close_mask) &&
	    tail_has_to_be_packed(inode)) {

		/* if regular file is released by last holder and it has been
		   appended (we append by unformatted node only) or its direct
		   item(s) had to be converted, then it may have to be
		   indirect2direct converted */
		err = reiserfs_truncate_file(inode, 0);
	}
      out:
	reiserfs_write_unlock(inode->i_sb);
	mutex_unlock(&(REISERFS_I(inode)->tailpack));
	return err;
}
