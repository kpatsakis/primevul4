static int mailimf_addr_spec_parse(const char * message, size_t length,
				   size_t * indx,
				   char ** result)
{
  size_t cur_token;
#if 0
  char * local_part;
  char * domain;
#endif
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
    case ',':
    case '\r':
    case '\n':
    case '(':
    case ')':
    case ':':
    case ';':
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
  
#if 0
  strncpy(addr_spec, message + cur_token, end - cur_token);
  addr_spec[end - cur_token] = '\0';
#endif

  cur_token = end;

#if 0
  r = mailimf_local_part_parse(message, length, &cur_token, &local_part);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_at_sign_parse(message, length, &cur_token);
  switch (r) {
  case MAILIMF_NO_ERROR:
    r = mailimf_domain_parse(message, length, &cur_token, &domain);
    if (r != MAILIMF_NO_ERROR) {
      res = r;
      goto free_local_part;
    }
    break;

  case MAILIMF_ERROR_PARSE:
    domain = NULL;
    break;

  default:
    res = r;
    goto free_local_part;
  }

  if (domain) {
    addr_spec = malloc(strlen(local_part) + strlen(domain) + 2);
    if (addr_spec == NULL) {
      res = MAILIMF_ERROR_MEMORY;
      goto free_domain;
    }
    
    strcpy(addr_spec, local_part);
    strcat(addr_spec, "@");
    strcat(addr_spec, domain);

    mailimf_domain_free(domain);
    mailimf_local_part_free(local_part);
  }
  else {
    addr_spec = local_part;
  }
#endif

  * result = addr_spec;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

#if 0
 free_domain:
  mailimf_domain_free(domain);
 free_local_part:
  mailimf_local_part_free(local_part);
#endif
 err:
  return res;
}
