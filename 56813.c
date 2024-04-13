SIZED_STRING* yr_object_get_string(
    YR_OBJECT* object,
    const char* field,
    ...)
{
  YR_OBJECT* string_obj;

  va_list args;
  va_start(args, field);

  if (field != NULL)
    string_obj = _yr_object_lookup(object, 0, field, args);
  else
    string_obj = object;

  va_end(args);

  if (string_obj == NULL)
    return NULL;

  assertf(string_obj->type == OBJECT_TYPE_STRING,
          "type of \"%s\" is not string\n", field);

  return string_obj->value.ss;
}
