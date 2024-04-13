void fuse_release_common(struct file *file, int opcode)
{
	struct fuse_file *ff;
	struct fuse_req *req;

	ff = file->private_data;
	if (unlikely(!ff))
		return;

	req = ff->reserved_req;
	fuse_prepare_release(ff, file->f_flags, opcode);

	if (ff->flock) {
		struct fuse_release_in *inarg = &req->misc.release.in;
		inarg->release_flags |= FUSE_RELEASE_FLOCK_UNLOCK;
		inarg->lock_owner = fuse_lock_owner_id(ff->fc,
						       (fl_owner_t) file);
	}
	/* Hold inode until release is finished */
	req->misc.release.inode = igrab(file_inode(file));

	/*
	 * Normally this will send the RELEASE request, however if
	 * some asynchronous READ or WRITE requests are outstanding,
	 * the sending will be delayed.
	 *
	 * Make the release synchronous if this is a fuseblk mount,
	 * synchronous RELEASE is allowed (and desirable) in this case
	 * because the server can be trusted not to screw up.
	 */
	fuse_file_put(ff, ff->fc->destroy_req != NULL);
}
