ppp_find_unit(struct ppp_net *pn, int unit)
{
	return unit_find(&pn->units_idr, unit);
}
