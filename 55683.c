expat_end_cb(void *userData, const XML_Char *name)
{
	struct expat_userData *ud = (struct expat_userData *)userData;

	xml_end(ud->archive, (const char *)name);
}
