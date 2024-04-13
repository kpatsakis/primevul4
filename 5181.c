void dimC_box_del(GF_Box *s)
{
	GF_DIMSSceneConfigBox *p = (GF_DIMSSceneConfigBox *)s;
	if (p->contentEncoding) gf_free(p->contentEncoding);
	if (p->textEncoding) gf_free(p->textEncoding);
	gf_free(p);
}