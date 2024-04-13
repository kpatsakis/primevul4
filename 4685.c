void fmtutil_default_iff_chunk_identify(deark *c, struct de_iffctx *ictx)
{
	const char *name = NULL;

	switch(ictx->chunkctx->chunk4cc.id) {
	case CODE__c_ : name="copyright"; break;
	case CODE_ANNO: name="annotation"; break;
	case CODE_AUTH: name="author"; break;
	}

	if(name) {
		ictx->chunkctx->chunk_name = name;
	}
}