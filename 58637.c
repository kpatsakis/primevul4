static inline void dentry_unlist(struct dentry *dentry, struct dentry *parent)
{
	struct dentry *next;
	/*
	 * Inform d_walk() and shrink_dentry_list() that we are no longer
	 * attached to the dentry tree
	 */
	dentry->d_flags |= DCACHE_DENTRY_KILLED;
	if (unlikely(list_empty(&dentry->d_child)))
		return;
	__list_del_entry(&dentry->d_child);
	/*
	 * Cursors can move around the list of children.  While we'd been
	 * a normal list member, it didn't matter - ->d_child.next would've
	 * been updated.  However, from now on it won't be and for the
	 * things like d_walk() it might end up with a nasty surprise.
	 * Normally d_walk() doesn't care about cursors moving around -
	 * ->d_lock on parent prevents that and since a cursor has no children
	 * of its own, we get through it without ever unlocking the parent.
	 * There is one exception, though - if we ascend from a child that
	 * gets killed as soon as we unlock it, the next sibling is found
	 * using the value left in its ->d_child.next.  And if _that_
	 * pointed to a cursor, and cursor got moved (e.g. by lseek())
	 * before d_walk() regains parent->d_lock, we'll end up skipping
	 * everything the cursor had been moved past.
	 *
	 * Solution: make sure that the pointer left behind in ->d_child.next
	 * points to something that won't be moving around.  I.e. skip the
	 * cursors.
	 */
	while (dentry->d_child.next != &parent->d_subdirs) {
		next = list_entry(dentry->d_child.next, struct dentry, d_child);
		if (likely(!(next->d_flags & DCACHE_DENTRY_CURSOR)))
			break;
		dentry->d_child.next = next->d_child.next;
	}
}
