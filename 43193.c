void udf_write_aext(struct inode *inode, struct extent_position *epos,
		    struct kernel_lb_addr *eloc, uint32_t elen, int inc)
{
	int adsize;
	uint8_t *ptr;
	struct short_ad *sad;
	struct long_ad *lad;
	struct udf_inode_info *iinfo = UDF_I(inode);

	if (!epos->bh)
		ptr = iinfo->i_ext.i_data + epos->offset -
			udf_file_entry_alloc_offset(inode) +
			iinfo->i_lenEAttr;
	else
		ptr = epos->bh->b_data + epos->offset;

	switch (iinfo->i_alloc_type) {
	case ICBTAG_FLAG_AD_SHORT:
		sad = (struct short_ad *)ptr;
		sad->extLength = cpu_to_le32(elen);
		sad->extPosition = cpu_to_le32(eloc->logicalBlockNum);
		adsize = sizeof(struct short_ad);
		break;
	case ICBTAG_FLAG_AD_LONG:
		lad = (struct long_ad *)ptr;
		lad->extLength = cpu_to_le32(elen);
		lad->extLocation = cpu_to_lelb(*eloc);
		memset(lad->impUse, 0x00, sizeof(lad->impUse));
		adsize = sizeof(struct long_ad);
		break;
	default:
		return;
	}

	if (epos->bh) {
		if (!UDF_QUERY_FLAG(inode->i_sb, UDF_FLAG_STRICT) ||
		    UDF_SB(inode->i_sb)->s_udfrev >= 0x0201) {
			struct allocExtDesc *aed =
				(struct allocExtDesc *)epos->bh->b_data;
			udf_update_tag(epos->bh->b_data,
				       le32_to_cpu(aed->lengthAllocDescs) +
				       sizeof(struct allocExtDesc));
		}
		mark_buffer_dirty_inode(epos->bh, inode);
	} else {
		mark_inode_dirty(inode);
	}

	if (inc)
		epos->offset += adsize;
}
