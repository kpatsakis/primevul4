static bool dccp_invert_tuple(struct nf_conntrack_tuple *inv,
			      const struct nf_conntrack_tuple *tuple)
{
	inv->src.u.dccp.port = tuple->dst.u.dccp.port;
	inv->dst.u.dccp.port = tuple->src.u.dccp.port;
	return true;
}
