static void udf_update_extents(struct inode *inode,
			       struct kernel_long_ad laarr[EXTENT_MERGE_SIZE],
			       int startnum, int endnum,
			       struct extent_position *epos)
{
	int start = 0, i;
	struct kernel_lb_addr tmploc;
	uint32_t tmplen;

	if (startnum > endnum) {
		for (i = 0; i < (startnum - endnum); i++)
			udf_delete_aext(inode, *epos, laarr[i].extLocation,
					laarr[i].extLength);
	} else if (startnum < endnum) {
		for (i = 0; i < (endnum - startnum); i++) {
			udf_insert_aext(inode, *epos, laarr[i].extLocation,
					laarr[i].extLength);
			udf_next_aext(inode, epos, &laarr[i].extLocation,
				      &laarr[i].extLength, 1);
			start++;
		}
	}

	for (i = start; i < endnum; i++) {
		udf_next_aext(inode, epos, &tmploc, &tmplen, 0);
		udf_write_aext(inode, epos, &laarr[i].extLocation,
			       laarr[i].extLength, 1);
	}
}
