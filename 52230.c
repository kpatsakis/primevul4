mtree_atol(char **p)
{
	if (**p != '0')
		return mtree_atol10(p);
	if ((*p)[1] == 'x' || (*p)[1] == 'X') {
		*p += 2;
		return mtree_atol16(p);
	}
	return mtree_atol8(p);
}
