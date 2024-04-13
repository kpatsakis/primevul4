static void filename_compute_type(struct policydb *p, struct context *newcontext,
				  u32 stype, u32 ttype, u16 tclass,
				  const char *objname)
{
	struct filename_trans ft;
	struct filename_trans_datum *otype;

	/*
	 * Most filename trans rules are going to live in specific directories
	 * like /dev or /var/run.  This bitmap will quickly skip rule searches
	 * if the ttype does not contain any rules.
	 */
	if (!ebitmap_get_bit(&p->filename_trans_ttypes, ttype))
		return;

	ft.stype = stype;
	ft.ttype = ttype;
	ft.tclass = tclass;
	ft.name = objname;

	otype = hashtab_search(p->filename_trans, &ft);
	if (otype)
		newcontext->type = otype->otype;
}
