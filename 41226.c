static void unlock_mount(struct path *path)
{
	up_write(&namespace_sem);
	mutex_unlock(&path->dentry->d_inode->i_mutex);
}
