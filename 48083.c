dtls1_copy_record(SSL *s, pitem *item)
    {
    DTLS1_RECORD_DATA *rdata;

    rdata = (DTLS1_RECORD_DATA *)item->data;
    
    if (s->s3->rbuf.buf != NULL)
        OPENSSL_free(s->s3->rbuf.buf);
    
    s->packet = rdata->packet;
    s->packet_length = rdata->packet_length;
    memcpy(&(s->s3->rbuf), &(rdata->rbuf), sizeof(SSL3_BUFFER));
    memcpy(&(s->s3->rrec), &(rdata->rrec), sizeof(SSL3_RECORD));
	
	/* Set proper sequence number for mac calculation */
	memcpy(&(s->s3->read_sequence[2]), &(rdata->packet[5]), 6);
    
    return(1);
    }
