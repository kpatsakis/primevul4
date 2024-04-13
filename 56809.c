int yr_object_function_create(
    const char* identifier,
    const char* arguments_fmt,
    const char* return_fmt,
    YR_MODULE_FUNC code,
    YR_OBJECT* parent,
    YR_OBJECT** function)
{
  YR_OBJECT* return_obj;
  YR_OBJECT* o = NULL;
  YR_OBJECT_FUNCTION* f = NULL;

  int8_t return_type;
  int i;

  switch (*return_fmt)
  {
    case 'i':
      return_type = OBJECT_TYPE_INTEGER;
      break;
    case 's':
      return_type = OBJECT_TYPE_STRING;
      break;
    case 'f':
      return_type = OBJECT_TYPE_FLOAT;
      break;
    default:
      return ERROR_INVALID_FORMAT;
  }

  if (parent != NULL)
  {

    assert(parent->type == OBJECT_TYPE_STRUCTURE);


    f = object_as_function(yr_object_lookup_field(parent, identifier));


    if (f != NULL && return_type != f->return_obj->type)
      return ERROR_WRONG_RETURN_TYPE;
  }

  if (f == NULL) // Function doesn't exist yet
  {
    FAIL_ON_ERROR(
        yr_object_create(
            OBJECT_TYPE_FUNCTION,
            identifier,
            parent,
            &o));

    FAIL_ON_ERROR_WITH_CLEANUP(
        yr_object_create(
            return_type,
            "result",
            o,
            &return_obj),
        yr_object_destroy(o));

    f = object_as_function(o);
  }

  for (i = 0; i < MAX_OVERLOADED_FUNCTIONS; i++)
  {
    if (f->prototypes[i].arguments_fmt == NULL)
    {
      f->prototypes[i].arguments_fmt = arguments_fmt;
      f->prototypes[i].code = code;

      break;
    }
  }

  if (function != NULL)
    *function = (YR_OBJECT*) f;

  return ERROR_SUCCESS;
}
