posix_acl_to_xattr(struct user_namespace *user_ns, const struct posix_acl *acl,
		   void *buffer, size_t size)
{
	struct posix_acl_xattr_header *ext_acl = buffer;
	struct posix_acl_xattr_entry *ext_entry;
	int real_size, n;

	real_size = posix_acl_xattr_size(acl->a_count);
	if (!buffer)
		return real_size;
	if (real_size > size)
		return -ERANGE;

	ext_entry = (void *)(ext_acl + 1);
	ext_acl->a_version = cpu_to_le32(POSIX_ACL_XATTR_VERSION);

	for (n=0; n < acl->a_count; n++, ext_entry++) {
		const struct posix_acl_entry *acl_e = &acl->a_entries[n];
		ext_entry->e_tag  = cpu_to_le16(acl_e->e_tag);
		ext_entry->e_perm = cpu_to_le16(acl_e->e_perm);
		switch(acl_e->e_tag) {
		case ACL_USER:
			ext_entry->e_id =
				cpu_to_le32(from_kuid(user_ns, acl_e->e_uid));
			break;
		case ACL_GROUP:
			ext_entry->e_id =
				cpu_to_le32(from_kgid(user_ns, acl_e->e_gid));
			break;
		default:
			ext_entry->e_id = cpu_to_le32(ACL_UNDEFINED_ID);
			break;
		}
	}
	return real_size;
}
