static void dentry_lock_for_move(struct dentry *dentry, struct dentry *target)
{
	/*
	 * XXXX: do we really need to take target->d_lock?
	 */
	if (IS_ROOT(dentry) || dentry->d_parent == target->d_parent)
		spin_lock(&target->d_parent->d_lock);
	else {
		if (d_ancestor(dentry->d_parent, target->d_parent)) {
			spin_lock(&dentry->d_parent->d_lock);
			spin_lock_nested(&target->d_parent->d_lock,
						DENTRY_D_LOCK_NESTED);
		} else {
			spin_lock(&target->d_parent->d_lock);
			spin_lock_nested(&dentry->d_parent->d_lock,
						DENTRY_D_LOCK_NESTED);
		}
	}
	if (target < dentry) {
		spin_lock_nested(&target->d_lock, 2);
		spin_lock_nested(&dentry->d_lock, 3);
	} else {
		spin_lock_nested(&dentry->d_lock, 2);
		spin_lock_nested(&target->d_lock, 3);
	}
}
