void gppc_box_del(GF_Box *s)
{
	GF_3GPPConfigBox *ptr = (GF_3GPPConfigBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}