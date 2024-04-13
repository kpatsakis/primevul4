expat_data_cb(void *userData, const XML_Char *s, int len)
{
	struct expat_userData *ud = (struct expat_userData *)userData;

	xml_data(ud->archive, s, len);
}
