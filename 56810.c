double yr_object_get_float(
    YR_OBJECT* object,
    const char* field,
    ...)
{
  YR_OBJECT* double_obj;

  va_list args;
  va_start(args, field);

  if (field != NULL)
    double_obj = _yr_object_lookup(object, 0, field, args);
  else
    double_obj = object;

  va_end(args);

  if (double_obj == NULL)
    return NAN;

  assertf(double_obj->type == OBJECT_TYPE_FLOAT,
          "type of \"%s\" is not double\n", field);

  return double_obj->value.d;
}
