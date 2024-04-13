rd_parse_cdb(struct se_cmd *cmd)
{
	return sbc_parse_cdb(cmd, &rd_sbc_ops);
}
