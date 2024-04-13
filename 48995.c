int expwrite(off_t a, char *buf, size_t len, CLIENT *client, int fua) {
	char pagebuf[DIFFPAGESIZE];
	off_t mapcnt,mapl,maph;
	off_t wrlen,rdlen; 
	off_t pagestart;
	off_t offset;

	if (!(client->server->flags & F_COPYONWRITE))
		return(rawexpwrite_fully(a, buf, len, client, fua)); 
	DEBUG("Asked to write %u bytes at %llu.\n", (unsigned int)len, (unsigned long long)a);

	mapl=a/DIFFPAGESIZE ; maph=(a+len-1)/DIFFPAGESIZE ;

	for (mapcnt=mapl;mapcnt<=maph;mapcnt++) {
		pagestart=mapcnt*DIFFPAGESIZE ;
		offset=a-pagestart ;
		wrlen=(0<DIFFPAGESIZE-offset && len<(size_t)(DIFFPAGESIZE-offset)) ?
			len : (size_t)DIFFPAGESIZE-offset;

		if (client->difmap[mapcnt]!=(u32)(-1)) { /* the block is already there */
			DEBUG("Page %llu is at %lu\n", (unsigned long long)mapcnt,
			       (unsigned long)(client->difmap[mapcnt])) ;
			myseek(client->difffile,
					client->difmap[mapcnt]*DIFFPAGESIZE+offset);
			if (write(client->difffile, buf, wrlen) != wrlen) return -1 ;
		} else { /* the block is not there */
			myseek(client->difffile,client->difffilelen*DIFFPAGESIZE) ;
			client->difmap[mapcnt]=(client->server->flags&F_SPARSE)?mapcnt:client->difffilelen++;
			DEBUG("Page %llu is not here, we put it at %lu\n",
			       (unsigned long long)mapcnt,
			       (unsigned long)(client->difmap[mapcnt]));
			rdlen=DIFFPAGESIZE ;
			if (rawexpread_fully(pagestart, pagebuf, rdlen, client))
				return -1;
			memcpy(pagebuf+offset,buf,wrlen) ;
			if (write(client->difffile, pagebuf, DIFFPAGESIZE) !=
					DIFFPAGESIZE)
				return -1;
		}						    
		len-=wrlen ; a+=wrlen ; buf+=wrlen ;
	}
	if (client->server->flags & F_SYNC) {
		fsync(client->difffile);
	} else if (fua) {
		/* open question: would it be cheaper to do multiple sync_file_ranges?
		   as we iterate through the above?
		 */
		fdatasync(client->difffile);
	}
	return 0;
}
