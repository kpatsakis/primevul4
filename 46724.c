static int set_bitmap_file(struct mddev *mddev, int fd)
{
	int err = 0;

	if (mddev->pers) {
		if (!mddev->pers->quiesce || !mddev->thread)
			return -EBUSY;
		if (mddev->recovery || mddev->sync_thread)
			return -EBUSY;
		/* we should be able to change the bitmap.. */
	}

	if (fd >= 0) {
		struct inode *inode;
		struct file *f;

		if (mddev->bitmap || mddev->bitmap_info.file)
			return -EEXIST; /* cannot add when bitmap is present */
		f = fget(fd);

		if (f == NULL) {
			printk(KERN_ERR "%s: error: failed to get bitmap file\n",
			       mdname(mddev));
			return -EBADF;
		}

		inode = f->f_mapping->host;
		if (!S_ISREG(inode->i_mode)) {
			printk(KERN_ERR "%s: error: bitmap file must be a regular file\n",
			       mdname(mddev));
			err = -EBADF;
		} else if (!(f->f_mode & FMODE_WRITE)) {
			printk(KERN_ERR "%s: error: bitmap file must open for write\n",
			       mdname(mddev));
			err = -EBADF;
		} else if (atomic_read(&inode->i_writecount) != 1) {
			printk(KERN_ERR "%s: error: bitmap file is already in use\n",
			       mdname(mddev));
			err = -EBUSY;
		}
		if (err) {
			fput(f);
			return err;
		}
		mddev->bitmap_info.file = f;
		mddev->bitmap_info.offset = 0; /* file overrides offset */
	} else if (mddev->bitmap == NULL)
		return -ENOENT; /* cannot remove what isn't there */
	err = 0;
	if (mddev->pers) {
		mddev->pers->quiesce(mddev, 1);
		if (fd >= 0) {
			struct bitmap *bitmap;

			bitmap = bitmap_create(mddev, -1);
			if (!IS_ERR(bitmap)) {
				mddev->bitmap = bitmap;
				err = bitmap_load(mddev);
			} else
				err = PTR_ERR(bitmap);
		}
		if (fd < 0 || err) {
			bitmap_destroy(mddev);
			fd = -1; /* make sure to put the file */
		}
		mddev->pers->quiesce(mddev, 0);
	}
	if (fd < 0) {
		struct file *f = mddev->bitmap_info.file;
		if (f) {
			spin_lock(&mddev->lock);
			mddev->bitmap_info.file = NULL;
			spin_unlock(&mddev->lock);
			fput(f);
		}
	}

	return err;
}
