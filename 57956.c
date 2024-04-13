int mailimf_msg_id_parse(const char * message, size_t length,
			 size_t * indx,
			 char ** result)
{
  size_t cur_token;
#if 0
  char * id_left;
  char * id_right;
#endif
  char * msg_id;
  int r;
  int res;

  cur_token = * indx;

  r = mailimf_cfws_parse(message, length, &cur_token);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE))
    return r;

  r = mailimf_lower_parse(message, length, &cur_token);
  if (r == MAILIMF_ERROR_PARSE) {
    r = mailimf_addr_spec_msg_id_parse(message, length, &cur_token, &msg_id);
    if (r != MAILIMF_NO_ERROR) {
      res = r;
      goto err;
    }
    
    * result = msg_id;
    * indx = cur_token;
    
    return MAILIMF_NO_ERROR;
  }
  else if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_lower_parse(message, length, &cur_token);
  if (r == MAILIMF_NO_ERROR) {
  }
  else if (r == MAILIMF_ERROR_PARSE) {
  }
  else {
    res = r;
    goto err;
  }
  r = mailimf_addr_spec_msg_id_parse(message, length, &cur_token, &msg_id);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }
  
  r = mailimf_greater_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    free(msg_id);
    res = r;
    goto err;
  }
  r = mailimf_greater_parse(message, length, &cur_token);
  if (r == MAILIMF_NO_ERROR) {
  }
  else if (r == MAILIMF_ERROR_PARSE) {
  }
  else {
    free(msg_id);
    res = r;
    goto err;
  }
  
#if 0
  r = mailimf_id_left_parse(message, length, &cur_token, &id_left);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto err;
  }

  r = mailimf_at_sign_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_id_left;
  }

  r = mailimf_id_right_parse(message, length, &cur_token, &id_right);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_id_left;
  }

  r = mailimf_greater_parse(message, length, &cur_token);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_id_right;
  }

  msg_id = malloc(strlen(id_left) + strlen(id_right) + 2);
  if (msg_id == NULL) {
    res = MAILIMF_ERROR_MEMORY;
    goto free_id_right;
  }
  strcpy(msg_id, id_left);
  strcat(msg_id, "@");
  strcat(msg_id, id_right);

  mailimf_id_left_free(id_left);
  mailimf_id_right_free(id_right);
#endif

  * result = msg_id;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

#if 0
 free_id_right:
  mailimf_id_right_free(id_right);
 free_id_left:
  mailimf_id_left_free(id_left);
#endif
  /*
 free:
  mailimf_atom_free(msg_id);
  */
 err:
  return res;
}
