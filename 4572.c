
void gf_isom_box_dump_done(const char *name, GF_Box *ptr, FILE *trace)
{
	if (ptr && ptr->child_boxes) {
		gf_isom_box_array_dump(ptr->child_boxes, trace);
	}
	if (name)
		gf_fprintf(trace, "</%s>\n", name);