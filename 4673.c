int fmtutil_is_standard_iff_chunk(deark *c, struct de_iffctx *ictx,
	u32 ct)
{
	switch(ct) {
	case CODE__c_:
	case CODE_ANNO:
	case CODE_AUTH:
	case CODE_NAME:
	case CODE_TEXT:
		return 1;
	}
	return 0;
}