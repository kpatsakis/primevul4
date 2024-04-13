int nfs4_acl_bytes(int entries)
{
	return sizeof(struct nfs4_acl) + entries * sizeof(struct nfs4_ace);
}
