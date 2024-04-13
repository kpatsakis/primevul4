static void evm_reset_status(struct inode *inode)
{
	struct integrity_iint_cache *iint;

	iint = integrity_iint_find(inode);
	if (iint)
		iint->evm_status = INTEGRITY_UNKNOWN;
}
