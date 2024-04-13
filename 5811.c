const GF_FilterRegister *isoffin_register(GF_FilterSession *session)
{
#ifdef GPAC_DISABLE_ISOM
	return NULL;
#else
	return &ISOFFInRegister;
#endif /*GPAC_DISABLE_ISOM*/
}