
GF_Err gf_isom_box_dump(void *ptr, FILE * trace)
{
	GF_Box *a = (GF_Box *) ptr;

	if (!a) {
		gf_fprintf(trace, "<!--ERROR: NULL Box Found-->\n");
		return GF_OK;
	}
	if (!a->registry) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[isom] trying to dump box %s not registered\n", gf_4cc_to_str(a->type) ));
		return GF_ISOM_INVALID_FILE;
	}
	a->registry->dump_fn(a, trace);
	return GF_OK;