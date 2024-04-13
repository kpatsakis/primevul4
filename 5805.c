static void isor_reset_seq_list(GF_List *list)
{
	while (gf_list_count(list)) {
		GF_NALUFFParam *sl = gf_list_pop_back(list);
		gf_free(sl->data);
		gf_free(sl);
	}
}