int8_t inode_bmap(struct inode *inode, sector_t block,
		  struct extent_position *pos, struct kernel_lb_addr *eloc,
		  uint32_t *elen, sector_t *offset)
{
	unsigned char blocksize_bits = inode->i_sb->s_blocksize_bits;
	loff_t lbcount = 0, bcount =
	    (loff_t) block << blocksize_bits;
	int8_t etype;
	struct udf_inode_info *iinfo;

	iinfo = UDF_I(inode);
	if (!udf_read_extent_cache(inode, bcount, &lbcount, pos)) {
		pos->offset = 0;
		pos->block = iinfo->i_location;
		pos->bh = NULL;
	}
	*elen = 0;
	do {
		etype = udf_next_aext(inode, pos, eloc, elen, 1);
		if (etype == -1) {
			*offset = (bcount - lbcount) >> blocksize_bits;
			iinfo->i_lenExtents = lbcount;
			return -1;
		}
		lbcount += *elen;
	} while (lbcount <= bcount);
	/* update extent cache */
	udf_update_extent_cache(inode, lbcount - *elen, pos, 1);
	*offset = (bcount + *elen - lbcount) >> blocksize_bits;

	return etype;
}
