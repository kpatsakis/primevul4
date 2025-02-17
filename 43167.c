static int udf_alloc_i_data(struct inode *inode, size_t size)
{
	struct udf_inode_info *iinfo = UDF_I(inode);
	iinfo->i_ext.i_data = kmalloc(size, GFP_KERNEL);

	if (!iinfo->i_ext.i_data) {
		udf_err(inode->i_sb, "(ino %ld) no free memory\n",
			inode->i_ino);
		return -ENOMEM;
	}

	return 0;
}
