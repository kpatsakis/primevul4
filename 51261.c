static char* dup_nickname(struct Curl_easy *data, enum dupstring cert_kind)
{
  const char *str = data->set.str[cert_kind];
  const char *n;

  if(!is_file(str))
    /* no such file exists, use the string as nickname */
    return strdup(str);

  /* search the first slash; we require at least one slash in a file name */
  n = strchr(str, '/');
  if(!n) {
    infof(data, "warning: certificate file name \"%s\" handled as nickname; "
          "please use \"./%s\" to force file name\n", str, str);
    return strdup(str);
  }

  /* we'll use the PEM reader to read the certificate from file */
  return NULL;
}
