static void init_rock_state(struct rock_state *rs, struct inode *inode)
{
	memset(rs, 0, sizeof(*rs));
 	rs->inode = inode;
 }
