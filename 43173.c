int8_t udf_delete_aext(struct inode *inode, struct extent_position epos,
		       struct kernel_lb_addr eloc, uint32_t elen)
{
	struct extent_position oepos;
	int adsize;
	int8_t etype;
	struct allocExtDesc *aed;
	struct udf_inode_info *iinfo;

	if (epos.bh) {
		get_bh(epos.bh);
		get_bh(epos.bh);
	}

	iinfo = UDF_I(inode);
	if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_SHORT)
		adsize = sizeof(struct short_ad);
	else if (iinfo->i_alloc_type == ICBTAG_FLAG_AD_LONG)
		adsize = sizeof(struct long_ad);
	else
		adsize = 0;

	oepos = epos;
	if (udf_next_aext(inode, &epos, &eloc, &elen, 1) == -1)
		return -1;

	while ((etype = udf_next_aext(inode, &epos, &eloc, &elen, 1)) != -1) {
		udf_write_aext(inode, &oepos, &eloc, (etype << 30) | elen, 1);
		if (oepos.bh != epos.bh) {
			oepos.block = epos.block;
			brelse(oepos.bh);
			get_bh(epos.bh);
			oepos.bh = epos.bh;
			oepos.offset = epos.offset - adsize;
		}
	}
	memset(&eloc, 0x00, sizeof(struct kernel_lb_addr));
	elen = 0;

	if (epos.bh != oepos.bh) {
		udf_free_blocks(inode->i_sb, inode, &epos.block, 0, 1);
		udf_write_aext(inode, &oepos, &eloc, elen, 1);
		udf_write_aext(inode, &oepos, &eloc, elen, 1);
		if (!oepos.bh) {
			iinfo->i_lenAlloc -= (adsize * 2);
			mark_inode_dirty(inode);
		} else {
			aed = (struct allocExtDesc *)oepos.bh->b_data;
			le32_add_cpu(&aed->lengthAllocDescs, -(2 * adsize));
			if (!UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT) ||
			    UDF_SB(inode->i_sb)->s_udfrev >= 0x0201)
				udf_update_tag(oepos.bh->b_data,
						oepos.offset - (2 * adsize));
			else
				udf_update_tag(oepos.bh->b_data,
						sizeof(struct allocExtDesc));
			mark_buffer_dirty_inode(oepos.bh, inode);
		}
	} else {
		udf_write_aext(inode, &oepos, &eloc, elen, 1);
		if (!oepos.bh) {
			iinfo->i_lenAlloc -= adsize;
			mark_inode_dirty(inode);
		} else {
			aed = (struct allocExtDesc *)oepos.bh->b_data;
			le32_add_cpu(&aed->lengthAllocDescs, -adsize);
			if (!UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT) ||
			    UDF_SB(inode->i_sb)->s_udfrev >= 0x0201)
				udf_update_tag(oepos.bh->b_data,
						epos.offset - adsize);
			else
				udf_update_tag(oepos.bh->b_data,
						sizeof(struct allocExtDesc));
			mark_buffer_dirty_inode(oepos.bh, inode);
		}
	}

	brelse(epos.bh);
	brelse(oepos.bh);

	return (elen >> 30);
}
