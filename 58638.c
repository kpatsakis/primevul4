static void dentry_unlock_for_move(struct dentry *dentry, struct dentry *target)
{
	if (target->d_parent != dentry->d_parent)
		spin_unlock(&dentry->d_parent->d_lock);
	if (target->d_parent != target)
		spin_unlock(&target->d_parent->d_lock);
	spin_unlock(&target->d_lock);
	spin_unlock(&dentry->d_lock);
}
