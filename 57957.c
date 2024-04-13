static int mailimf_name_addr_parse(const char * message, size_t length,
				   size_t * indx,
				   char ** pdisplay_name,
				   char ** pangle_addr)
{
  char * display_name;
  char * angle_addr;
  size_t cur_token;
  int r;
  int res;

  cur_token = * indx;

  display_name = NULL;
  angle_addr = NULL;

  r = mailimf_display_name_parse(message, length, &cur_token, &display_name);
  if ((r != MAILIMF_NO_ERROR) && (r != MAILIMF_ERROR_PARSE)) {
    res = r;
    goto err;
  }

  r = mailimf_angle_addr_parse(message, length, &cur_token, &angle_addr);
  if (r != MAILIMF_NO_ERROR) {
    res = r;
    goto free_display_name;
  }

  * pdisplay_name = display_name;
  * pangle_addr = angle_addr;
  * indx = cur_token;

  return MAILIMF_NO_ERROR;

 free_display_name:
  if (display_name != NULL)
    mailimf_display_name_free(display_name);
 err:
  return res;
}
