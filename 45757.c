static struct sock *unix_find_socket_byinode(struct inode *i)
{
	struct sock *s;

	spin_lock(&unix_table_lock);
	sk_for_each(s,
		    &unix_socket_table[i->i_ino & (UNIX_HASH_SIZE - 1)]) {
		struct dentry *dentry = unix_sk(s)->path.dentry;

		if (dentry && dentry->d_inode == i) {
			sock_hold(s);
			goto found;
		}
	}
	s = NULL;
found:
	spin_unlock(&unix_table_lock);
	return s;
}
