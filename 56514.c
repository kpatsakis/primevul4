static const struct sys_reg_desc *find_reg(const struct sys_reg_params *params,
					 const struct sys_reg_desc table[],
					 unsigned int num)
{
	unsigned long pval = reg_to_match_value(params);

	return bsearch((void *)pval, table, num, sizeof(table[0]), match_sys_reg);
}
