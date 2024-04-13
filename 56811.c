int64_t yr_object_get_integer(
    YR_OBJECT* object,
    const char* field,
    ...)
{
  YR_OBJECT* integer_obj;

  va_list args;
  va_start(args, field);

  if (field != NULL)
    integer_obj = _yr_object_lookup(object, 0, field, args);
  else
    integer_obj = object;

  va_end(args);

  if (integer_obj == NULL)
    return UNDEFINED;

  assertf(integer_obj->type == OBJECT_TYPE_INTEGER,
          "type of \"%s\" is not integer\n", field);

  return integer_obj->value.i;
}
