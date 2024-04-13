int yr_object_has_undefined_value(
    YR_OBJECT* object,
    const char* field,
    ...)
{
  YR_OBJECT* field_obj;

  va_list args;
  va_start(args, field);

  if (field != NULL)
    field_obj = _yr_object_lookup(object, 0, field, args);
  else
    field_obj = object;

  va_end(args);

  if (field_obj == NULL)
    return TRUE;

  switch(field_obj->type)
  {
    case OBJECT_TYPE_FLOAT:
      return isnan(field_obj->value.d);
    case OBJECT_TYPE_STRING:
      return field_obj->value.ss == NULL;
    case OBJECT_TYPE_INTEGER:
      return field_obj->value.i == UNDEFINED;
  }

  return FALSE;
}
