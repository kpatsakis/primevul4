static void hmac_add_misc(struct shash_desc *desc, struct inode *inode,
			  char *digest)
{
	struct h_misc {
		unsigned long ino;
		__u32 generation;
		uid_t uid;
		gid_t gid;
		umode_t mode;
	} hmac_misc;

	memset(&hmac_misc, 0, sizeof hmac_misc);
	hmac_misc.ino = inode->i_ino;
	hmac_misc.generation = inode->i_generation;
	hmac_misc.uid = from_kuid(&init_user_ns, inode->i_uid);
	hmac_misc.gid = from_kgid(&init_user_ns, inode->i_gid);
	hmac_misc.mode = inode->i_mode;
	crypto_shash_update(desc, (const u8 *)&hmac_misc, sizeof hmac_misc);
	crypto_shash_final(desc, digest);
}
