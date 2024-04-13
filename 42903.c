static int check_sp(struct rock_ridge *rr, struct inode *inode)
{
	if (rr->u.SP.magic[0] != 0xbe)
		return -1;
	if (rr->u.SP.magic[1] != 0xef)
		return -1;
	ISOFS_SB(inode->i_sb)->s_rock_offset = rr->u.SP.skip;
	return 0;
}
