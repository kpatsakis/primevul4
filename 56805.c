int yr_object_create(
    int8_t type,
    const char* identifier,
    YR_OBJECT* parent,
    YR_OBJECT** object)
{
  YR_OBJECT* obj;
  int i;
  size_t object_size = 0;

  assert(parent != NULL || object != NULL);

  switch (type)
  {
    case OBJECT_TYPE_STRUCTURE:
      object_size = sizeof(YR_OBJECT_STRUCTURE);
      break;
    case OBJECT_TYPE_ARRAY:
      object_size = sizeof(YR_OBJECT_ARRAY);
      break;
    case OBJECT_TYPE_DICTIONARY:
      object_size = sizeof(YR_OBJECT_DICTIONARY);
      break;
    case OBJECT_TYPE_INTEGER:
      object_size = sizeof(YR_OBJECT);
      break;
    case OBJECT_TYPE_FLOAT:
      object_size = sizeof(YR_OBJECT);
      break;
    case OBJECT_TYPE_STRING:
      object_size = sizeof(YR_OBJECT);
      break;
    case OBJECT_TYPE_FUNCTION:
      object_size = sizeof(YR_OBJECT_FUNCTION);
      break;
    default:
      assert(FALSE);
  }

  obj = (YR_OBJECT*) yr_malloc(object_size);

  if (obj == NULL)
    return ERROR_INSUFFICIENT_MEMORY;

  obj->type = type;
  obj->identifier = yr_strdup(identifier);
  obj->parent = parent;
  obj->data = NULL;

  switch(type)
  {
    case OBJECT_TYPE_INTEGER:
      obj->value.i = UNDEFINED;
      break;
    case OBJECT_TYPE_FLOAT:
      obj->value.d = NAN;
      break;
    case OBJECT_TYPE_STRING:
      obj->value.ss = NULL;
      break;
    case OBJECT_TYPE_STRUCTURE:
      object_as_structure(obj)->members = NULL;
      break;
    case OBJECT_TYPE_ARRAY:
      object_as_array(obj)->items = NULL;
      object_as_array(obj)->prototype_item = NULL;
      break;
    case OBJECT_TYPE_DICTIONARY:
      object_as_dictionary(obj)->items = NULL;
      object_as_dictionary(obj)->prototype_item = NULL;
      break;
    case OBJECT_TYPE_FUNCTION:
      object_as_function(obj)->return_obj = NULL;
      for (i = 0; i < MAX_OVERLOADED_FUNCTIONS; i++)
      {
        object_as_function(obj)->prototypes[i].arguments_fmt = NULL;
        object_as_function(obj)->prototypes[i].code = NULL;
      }
      break;
  }

  if (obj->identifier == NULL)
  {
    yr_free(obj);
    return ERROR_INSUFFICIENT_MEMORY;
  }

  if (parent != NULL)
  {
    assert(parent->type == OBJECT_TYPE_STRUCTURE ||
           parent->type == OBJECT_TYPE_ARRAY ||
           parent->type == OBJECT_TYPE_DICTIONARY ||
           parent->type == OBJECT_TYPE_FUNCTION);

    switch(parent->type)
    {
      case OBJECT_TYPE_STRUCTURE:
        FAIL_ON_ERROR_WITH_CLEANUP(
            yr_object_structure_set_member(parent, obj),
            {
              yr_free((void*) obj->identifier);
              yr_free(obj);
            });
        break;

      case OBJECT_TYPE_ARRAY:
        object_as_array(parent)->prototype_item = obj;
        break;

      case OBJECT_TYPE_DICTIONARY:
        object_as_dictionary(parent)->prototype_item = obj;
        break;

      case OBJECT_TYPE_FUNCTION:
        object_as_function(parent)->return_obj = obj;
        break;
    }
  }

  if (object != NULL)
    *object = obj;

  return ERROR_SUCCESS;
}
