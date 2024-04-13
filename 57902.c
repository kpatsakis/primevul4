static int mailimf_addr_spec_msg_id_parse(const char * message, size_t length,
                                          size_t * indx,
                                          char ** result)
{
    size_t cur_token;
    char * addr_spec;
    int r;
    int res;
    size_t begin;
    size_t end;
    int final;
    size_t count;
    const char * src;
    char * dest;
    size_t i;
    
    cur_token = * indx;
    
    r = mailimf_cfws_parse(message, length, &cur_token);
    if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
        res = r;
        goto err;
    }
    
    end = cur_token;
    if (end >= length) {
        res = MAILIMF_ERROR_PARSE;
        goto err;
    }
    
    begin = cur_token;
    
    final = FALSE;
    while (1) {
        switch (message[end]) {
            case '>':
            case '\r':
            case '\n':
                final = TRUE;
                break;
        }
        
        if (final)
            break;
        
        end ++;
        if (end >= length)
            break;
    }
    
    if (end == begin) {
        res = MAILIMF_ERROR_PARSE;
        goto err;
    }
    
    addr_spec = malloc(end - cur_token + 1);
    if (addr_spec == NULL) {
        res = MAILIMF_ERROR_MEMORY;
        goto err;
    }
    
    count = end - cur_token;
    src = message + cur_token;
    dest = addr_spec;
    for(i = 0 ; i < count ; i ++) {
        if ((* src != ' ') && (* src != '\t')) {
            * dest = * src;
            dest ++;
        }
        src ++;
    }
    * dest = '\0';
    
    cur_token = end;
    
    * result = addr_spec;
    * indx = cur_token;
    
    return MAILIMF_NO_ERROR;
    
err:
    return res;
}
