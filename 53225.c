ace2type(struct nfs4_ace *ace)
{
	switch (ace->whotype) {
		case NFS4_ACL_WHO_NAMED:
			return (ace->flag & NFS4_ACE_IDENTIFIER_GROUP ?
					ACL_GROUP : ACL_USER);
		case NFS4_ACL_WHO_OWNER:
			return ACL_USER_OBJ;
		case NFS4_ACL_WHO_GROUP:
			return ACL_GROUP_OBJ;
		case NFS4_ACL_WHO_EVERYONE:
			return ACL_OTHER;
	}
	BUG();
	return -1;
}
