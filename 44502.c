static int dccp_print_conntrack(struct seq_file *s, struct nf_conn *ct)
{
	return seq_printf(s, "%s ", dccp_state_names[ct->proto.dccp.state]);
}
