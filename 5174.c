GF_Err dimC_box_size(GF_Box *s)
{
	GF_DIMSSceneConfigBox *p = (GF_DIMSSceneConfigBox *)s;
    s->size += 3 + 2;
    if (p->textEncoding) s->size += strlen(p->textEncoding);
    if (p->contentEncoding) s->size += strlen(p->contentEncoding);
	return GF_OK;
}