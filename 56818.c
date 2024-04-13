int yr_object_set_float(
    double value,
    YR_OBJECT* object,
    const char* field,
    ...)
{
  YR_OBJECT* double_obj;

  va_list args;
  va_start(args, field);

  if (field != NULL)
    double_obj = _yr_object_lookup(object, OBJECT_CREATE, field, args);
  else
    double_obj = object;

  va_end(args);

  assert(double_obj != NULL);
  assert(double_obj->type == OBJECT_TYPE_FLOAT);

  double_obj->value.d = value;

  return ERROR_SUCCESS;
}
