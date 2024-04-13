int evm_init_hmac(struct inode *inode, const struct xattr *lsm_xattr,
		  char *hmac_val)
{
	struct shash_desc *desc;

	desc = init_desc(EVM_XATTR_HMAC);
	if (IS_ERR(desc)) {
		printk(KERN_INFO "init_desc failed\n");
		return PTR_ERR(desc);
	}

	crypto_shash_update(desc, lsm_xattr->value, lsm_xattr->value_len);
	hmac_add_misc(desc, inode, hmac_val);
	kfree(desc);
	return 0;
}
