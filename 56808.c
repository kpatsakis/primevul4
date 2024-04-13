int yr_object_from_external_variable(
    YR_EXTERNAL_VARIABLE* external,
    YR_OBJECT** object)
{
  YR_OBJECT* obj;
  int result;
  uint8_t obj_type = 0;

  switch(external->type)
  {
    case EXTERNAL_VARIABLE_TYPE_INTEGER:
    case EXTERNAL_VARIABLE_TYPE_BOOLEAN:
      obj_type = OBJECT_TYPE_INTEGER;
      break;

    case EXTERNAL_VARIABLE_TYPE_FLOAT:
      obj_type = OBJECT_TYPE_FLOAT;
      break;

    case EXTERNAL_VARIABLE_TYPE_STRING:
    case EXTERNAL_VARIABLE_TYPE_MALLOC_STRING:
      obj_type = OBJECT_TYPE_STRING;
      break;

    default:
      assert(FALSE);
  }

  result = yr_object_create(
      obj_type,
      external->identifier,
      NULL,
      &obj);

  if (result == ERROR_SUCCESS)
  {
    switch(external->type)
    {
      case EXTERNAL_VARIABLE_TYPE_INTEGER:
      case EXTERNAL_VARIABLE_TYPE_BOOLEAN:
        yr_object_set_integer(external->value.i, obj, NULL);
        break;

      case EXTERNAL_VARIABLE_TYPE_FLOAT:
        yr_object_set_float(external->value.f, obj, NULL);
        break;

      case EXTERNAL_VARIABLE_TYPE_STRING:
      case EXTERNAL_VARIABLE_TYPE_MALLOC_STRING:
        yr_object_set_string(
            external->value.s, strlen(external->value.s), obj, NULL);
        break;
    }

    *object = obj;
  }

  return result;
}
