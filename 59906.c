rend_service_path(const rend_service_t *service, const char *file_name)
{
  char *file_path = NULL;

  tor_assert(service->directory);

  /* Can never fail: asserts rather than leaving file_path NULL. */
  tor_asprintf(&file_path, "%s%s%s",
               service->directory, PATH_SEPARATOR, file_name);

  return file_path;
}
