void vmhd_box_del(GF_Box *s)
{
	GF_VideoMediaHeaderBox *ptr = (GF_VideoMediaHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}