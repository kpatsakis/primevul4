struct inode *ovl_new_inode(struct super_block *sb, umode_t mode,
			    struct ovl_entry *oe)
{
	struct inode *inode;

	inode = new_inode(sb);
	if (!inode)
		return NULL;

	mode &= S_IFMT;

	inode->i_ino = get_next_ino();
	inode->i_mode = mode;
	inode->i_flags |= S_NOATIME | S_NOCMTIME;

	switch (mode) {
	case S_IFDIR:
		inode->i_private = oe;
		inode->i_op = &ovl_dir_inode_operations;
		inode->i_fop = &ovl_dir_operations;
		break;

	case S_IFLNK:
		inode->i_op = &ovl_symlink_inode_operations;
		break;

	case S_IFREG:
	case S_IFSOCK:
	case S_IFBLK:
	case S_IFCHR:
	case S_IFIFO:
		inode->i_op = &ovl_file_inode_operations;
		break;

	default:
		WARN(1, "illegal file type: %i\n", mode);
		iput(inode);
		inode = NULL;
	}

	return inode;
}
