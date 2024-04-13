static long cp_oldabi_stat64(struct kstat *stat,
			     struct oldabi_stat64 __user *statbuf)
{
	struct oldabi_stat64 tmp;

	tmp.st_dev = huge_encode_dev(stat->dev);
	tmp.__pad1 = 0;
	tmp.__st_ino = stat->ino;
	tmp.st_mode = stat->mode;
	tmp.st_nlink = stat->nlink;
	tmp.st_uid = from_kuid_munged(current_user_ns(), stat->uid);
	tmp.st_gid = from_kgid_munged(current_user_ns(), stat->gid);
	tmp.st_rdev = huge_encode_dev(stat->rdev);
	tmp.st_size = stat->size;
	tmp.st_blocks = stat->blocks;
	tmp.__pad2 = 0;
	tmp.st_blksize = stat->blksize;
	tmp.st_atime = stat->atime.tv_sec;
	tmp.st_atime_nsec = stat->atime.tv_nsec;
	tmp.st_mtime = stat->mtime.tv_sec;
	tmp.st_mtime_nsec = stat->mtime.tv_nsec;
	tmp.st_ctime = stat->ctime.tv_sec;
	tmp.st_ctime_nsec = stat->ctime.tv_nsec;
	tmp.st_ino = stat->ino;
	return copy_to_user(statbuf,&tmp,sizeof(tmp)) ? -EFAULT : 0;
}
