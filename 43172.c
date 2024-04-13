int8_t udf_current_aext(struct inode *inode, struct extent_position *epos,
			struct kernel_lb_addr *eloc, uint32_t *elen, int inc)
{
	int alen;
	int8_t etype;
	uint8_t *ptr;
	struct short_ad *sad;
	struct long_ad *lad;
	struct udf_inode_info *iinfo = UDF_I(inode);

	if (!epos->bh) {
		if (!epos->offset)
			epos->offset = udf_file_entry_alloc_offset(inode);
		ptr = iinfo->i_ext.i_data + epos->offset -
			udf_file_entry_alloc_offset(inode) +
			iinfo->i_lenEAttr;
		alen = udf_file_entry_alloc_offset(inode) +
							iinfo->i_lenAlloc;
	} else {
		if (!epos->offset)
			epos->offset = sizeof(struct allocExtDesc);
		ptr = epos->bh->b_data + epos->offset;
		alen = sizeof(struct allocExtDesc) +
			le32_to_cpu(((struct allocExtDesc *)epos->bh->b_data)->
							lengthAllocDescs);
	}

	switch (iinfo->i_alloc_type) {
	case ICBTAG_FLAG_AD_SHORT:
		sad = udf_get_fileshortad(ptr, alen, &epos->offset, inc);
		if (!sad)
			return -1;
		etype = le32_to_cpu(sad->extLength) >> 30;
		eloc->logicalBlockNum = le32_to_cpu(sad->extPosition);
		eloc->partitionReferenceNum =
				iinfo->i_location.partitionReferenceNum;
		*elen = le32_to_cpu(sad->extLength) & UDF_EXTENT_LENGTH_MASK;
		break;
	case ICBTAG_FLAG_AD_LONG:
		lad = udf_get_filelongad(ptr, alen, &epos->offset, inc);
		if (!lad)
			return -1;
		etype = le32_to_cpu(lad->extLength) >> 30;
		*eloc = lelb_to_cpu(lad->extLocation);
		*elen = le32_to_cpu(lad->extLength) & UDF_EXTENT_LENGTH_MASK;
		break;
	default:
		udf_debug("alloc_type = %d unsupported\n", iinfo->i_alloc_type);
		return -1;
	}

	return etype;
}
