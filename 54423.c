HTStream *HTMLToPlain(HTPresentation *pres,
		      HTParentAnchor *anchor,
		      HTStream *sink)
{
    CTRACE((tfp, "HTMLToPlain calling CacheThru_new\n"));
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTML_new(anchor, pres->rep_out, sink)));
}
