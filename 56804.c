int yr_object_copy(
    YR_OBJECT* object,
    YR_OBJECT** object_copy)
{
  YR_OBJECT* copy;
  YR_OBJECT* o;

  YR_STRUCTURE_MEMBER* structure_member;

  int i;

  *object_copy = NULL;

  FAIL_ON_ERROR(yr_object_create(
      object->type,
      object->identifier,
      NULL,
      &copy));

  switch(object->type)
  {
    case OBJECT_TYPE_INTEGER:
      copy->value.i = object->value.i;
      break;

    case OBJECT_TYPE_FLOAT:
      copy->value.d = object->value.d;
      break;

    case OBJECT_TYPE_STRING:

      if (object->value.ss != NULL)
        copy->value.ss = sized_string_dup(object->value.ss);
      else
        copy->value.ss = NULL;

      break;

    case OBJECT_TYPE_FUNCTION:

      FAIL_ON_ERROR_WITH_CLEANUP(
          yr_object_copy(
              object_as_function(object)->return_obj,
              &object_as_function(copy)->return_obj),
          yr_object_destroy(copy));

      for (i = 0; i < MAX_OVERLOADED_FUNCTIONS; i++)
        object_as_function(copy)->prototypes[i] = \
            object_as_function(object)->prototypes[i];

      break;

    case OBJECT_TYPE_STRUCTURE:

      structure_member = object_as_structure(object)->members;

      while (structure_member != NULL)
      {
        FAIL_ON_ERROR_WITH_CLEANUP(
            yr_object_copy(structure_member->object, &o),
            yr_object_destroy(copy));

        FAIL_ON_ERROR_WITH_CLEANUP(
            yr_object_structure_set_member(copy, o),
            yr_free(o);
            yr_object_destroy(copy));

        structure_member = structure_member->next;
      }

      break;

    case OBJECT_TYPE_ARRAY:

      FAIL_ON_ERROR_WITH_CLEANUP(
          yr_object_copy(object_as_array(object)->prototype_item, &o),
          yr_object_destroy(copy));

      object_as_array(copy)->prototype_item = o;

      break;

    case OBJECT_TYPE_DICTIONARY:

      FAIL_ON_ERROR_WITH_CLEANUP(
          yr_object_copy(object_as_dictionary(object)->prototype_item, &o),
          yr_object_destroy(copy));

      object_as_dictionary(copy)->prototype_item = o;

      break;

    default:
      assert(FALSE);

  }

  *object_copy = copy;

  return ERROR_SUCCESS;
}
