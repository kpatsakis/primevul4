static int exec_permission(struct inode *inode)
{
	int ret;

	if (inode->i_op->permission) {
		ret = inode->i_op->permission(inode, MAY_EXEC);
		if (!ret)
			goto ok;
		return ret;
	}
	ret = acl_permission_check(inode, MAY_EXEC, inode->i_op->check_acl);
	if (!ret)
		goto ok;

	if (capable(CAP_DAC_OVERRIDE) || capable(CAP_DAC_READ_SEARCH))
		goto ok;

	return ret;
ok:
	return security_inode_permission(inode, MAY_EXEC);
}
