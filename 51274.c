static const char* nss_error_to_name(PRErrorCode code)
{
  const char *name = PR_ErrorToName(code);
  if(name)
    return name;

  return "unknown error";
}
