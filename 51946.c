static const char *get_media_descr(unsigned char media)
{
    int i;

    for (i = 0; i < sizeof(mediabytes) / sizeof(*mediabytes); ++i) {
	if (mediabytes[i].media == media)
	    return (mediabytes[i].descr);
    }
    return ("undefined");
}
