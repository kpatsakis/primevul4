static int rock_ridge_symlink_readpage(struct file *file, struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct iso_inode_info *ei = ISOFS_I(inode);
	struct isofs_sb_info *sbi = ISOFS_SB(inode->i_sb);
	char *link = kmap(page);
	unsigned long bufsize = ISOFS_BUFFER_SIZE(inode);
	struct buffer_head *bh;
	char *rpnt = link;
	unsigned char *pnt;
	struct iso_directory_record *raw_de;
	unsigned long block, offset;
	int sig;
	struct rock_ridge *rr;
	struct rock_state rs;
	int ret;

	if (!sbi->s_rock)
		goto error;

	init_rock_state(&rs, inode);
	block = ei->i_iget5_block;
	bh = sb_bread(inode->i_sb, block);
	if (!bh)
		goto out_noread;

	offset = ei->i_iget5_offset;
	pnt = (unsigned char *)bh->b_data + offset;

	raw_de = (struct iso_directory_record *)pnt;

	/*
	 * If we go past the end of the buffer, there is some sort of error.
	 */
	if (offset + *pnt > bufsize)
		goto out_bad_span;

	/*
	 * Now test for possible Rock Ridge extensions which will override
	 * some of these numbers in the inode structure.
	 */

	setup_rock_ridge(raw_de, inode, &rs);

repeat:
	while (rs.len > 2) { /* There may be one byte for padding somewhere */
		rr = (struct rock_ridge *)rs.chr;
		if (rr->len < 3)
			goto out;	/* Something got screwed up here */
		sig = isonum_721(rs.chr);
		if (rock_check_overflow(&rs, sig))
			goto out;
		rs.chr += rr->len;
		rs.len -= rr->len;
		if (rs.len < 0)
			goto out;	/* corrupted isofs */

		switch (sig) {
		case SIG('R', 'R'):
			if ((rr->u.RR.flags[0] & RR_SL) == 0)
				goto out;
			break;
		case SIG('S', 'P'):
			if (check_sp(rr, inode))
				goto out;
			break;
		case SIG('S', 'L'):
			rpnt = get_symlink_chunk(rpnt, rr,
						 link + (PAGE_SIZE - 1));
			if (rpnt == NULL)
				goto out;
			break;
		case SIG('C', 'E'):
			/* This tells is if there is a continuation record */
			rs.cont_extent = isonum_733(rr->u.CE.extent);
			rs.cont_offset = isonum_733(rr->u.CE.offset);
			rs.cont_size = isonum_733(rr->u.CE.size);
		default:
			break;
		}
	}
	ret = rock_continue(&rs);
	if (ret == 0)
		goto repeat;
	if (ret < 0)
		goto fail;

	if (rpnt == link)
		goto fail;
	brelse(bh);
	*rpnt = '\0';
	SetPageUptodate(page);
	kunmap(page);
	unlock_page(page);
	return 0;

	/* error exit from macro */
out:
	kfree(rs.buffer);
	goto fail;
out_noread:
	printk("unable to read i-node block");
	goto fail;
out_bad_span:
	printk("symlink spans iso9660 blocks\n");
fail:
	brelse(bh);
error:
	SetPageError(page);
	kunmap(page);
	unlock_page(page);
	return -EIO;
}
