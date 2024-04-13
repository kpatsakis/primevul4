void IGDdata(void * d, const char * data, int l)
{
	struct IGDdatas * datas = (struct IGDdatas *)d;
	char * dstmember = 0;
	/*printf("%2d %s : %.*s\n",
           datas->level, datas->cureltname, l, data);	*/
	if( !strcmp(datas->cureltname, "URLBase") )
		dstmember = datas->urlbase;
	else if( !strcmp(datas->cureltname, "presentationURL") )
		dstmember = datas->presentationurl;
	else if( !strcmp(datas->cureltname, "serviceType") )
		dstmember = datas->tmp.servicetype;
	else if( !strcmp(datas->cureltname, "controlURL") )
		dstmember = datas->tmp.controlurl;
	else if( !strcmp(datas->cureltname, "eventSubURL") )
		dstmember = datas->tmp.eventsuburl;
	else if( !strcmp(datas->cureltname, "SCPDURL") )
		dstmember = datas->tmp.scpdurl;
/*	else if( !strcmp(datas->cureltname, "deviceType") )
		dstmember = datas->devicetype_tmp;*/
	if(dstmember)
	{
		if(l>=MINIUPNPC_URL_MAXSIZE)
			l = MINIUPNPC_URL_MAXSIZE-1;
		memcpy(dstmember, data, l);
		dstmember[l] = '\0';
	}
}
