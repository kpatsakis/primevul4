HTStream *HTMLPresent(HTPresentation *pres GCC_UNUSED,
		      HTParentAnchor *anchor,
		      HTStream *sink GCC_UNUSED)
{
    CTRACE((tfp, "HTMLPresent calling CacheThru_new\n"));
    return CacheThru_new(anchor,
			 SGML_new(&HTML_dtd, anchor,
				  HTML_new(anchor, WWW_PRESENT, NULL)));
}
