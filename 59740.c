static void dump_ace(struct cifs_ace *pace, char *end_of_acl)
{
	int num_subauth;

	/* validate that we do not go past end of acl */

	if (le16_to_cpu(pace->size) < 16) {
		cifs_dbg(VFS, "ACE too small %d\n", le16_to_cpu(pace->size));
		return;
	}

	if (end_of_acl < (char *)pace + le16_to_cpu(pace->size)) {
		cifs_dbg(VFS, "ACL too small to parse ACE\n");
		return;
	}

	num_subauth = pace->sid.num_subauth;
	if (num_subauth) {
		int i;
		cifs_dbg(FYI, "ACE revision %d num_auth %d type %d flags %d size %d\n",
			 pace->sid.revision, pace->sid.num_subauth, pace->type,
			 pace->flags, le16_to_cpu(pace->size));
		for (i = 0; i < num_subauth; ++i) {
			cifs_dbg(FYI, "ACE sub_auth[%d]: 0x%x\n",
				 i, le32_to_cpu(pace->sid.sub_auth[i]));
		}

		/* BB add length check to make sure that we do not have huge
			num auths and therefore go off the end */
	}

	return;
}
