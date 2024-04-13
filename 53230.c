deny_mask_from_posix(unsigned short perm, u32 flags)
{
	u32 mask = 0;

	if (perm & ACL_READ)
		mask |= NFS4_READ_MODE;
	if (perm & ACL_WRITE)
		mask |= NFS4_WRITE_MODE;
	if ((perm & ACL_WRITE) && (flags & NFS4_ACL_DIR))
		mask |= NFS4_ACE_DELETE_CHILD;
	if (perm & ACL_EXECUTE)
		mask |= NFS4_EXECUTE_MODE;
	return mask;
}
