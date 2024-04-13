static int dccp_print_tuple(struct seq_file *s,
			    const struct nf_conntrack_tuple *tuple)
{
	return seq_printf(s, "sport=%hu dport=%hu ",
			  ntohs(tuple->src.u.dccp.port),
			  ntohs(tuple->dst.u.dccp.port));
}
