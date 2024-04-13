static void posix_acl_fix_xattr_userns(
	struct user_namespace *to, struct user_namespace *from,
	void *value, size_t size)
{
	struct posix_acl_xattr_header *header = value;
	struct posix_acl_xattr_entry *entry = (void *)(header + 1), *end;
	int count;
	kuid_t uid;
	kgid_t gid;

	if (!value)
		return;
	if (size < sizeof(struct posix_acl_xattr_header))
		return;
	if (header->a_version != cpu_to_le32(POSIX_ACL_XATTR_VERSION))
		return;

	count = posix_acl_xattr_count(size);
	if (count < 0)
		return;
	if (count == 0)
		return;

	for (end = entry + count; entry != end; entry++) {
		switch(le16_to_cpu(entry->e_tag)) {
		case ACL_USER:
			uid = make_kuid(from, le32_to_cpu(entry->e_id));
			entry->e_id = cpu_to_le32(from_kuid(to, uid));
			break;
		case ACL_GROUP:
			gid = make_kgid(from, le32_to_cpu(entry->e_id));
			entry->e_id = cpu_to_le32(from_kgid(to, gid));
			break;
		default:
			break;
		}
	}
}
