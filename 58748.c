int vfs_readlink(struct dentry *dentry, char __user *buffer, int buflen)
{
	struct inode *inode = d_inode(dentry);

	if (unlikely(!(inode->i_opflags & IOP_DEFAULT_READLINK))) {
		if (unlikely(inode->i_op->readlink))
			return inode->i_op->readlink(dentry, buffer, buflen);

		if (!d_is_symlink(dentry))
			return -EINVAL;

		spin_lock(&inode->i_lock);
		inode->i_opflags |= IOP_DEFAULT_READLINK;
		spin_unlock(&inode->i_lock);
	}

	return generic_readlink(dentry, buffer, buflen);
}
