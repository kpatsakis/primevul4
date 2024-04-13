YR_OBJECT* yr_object_array_get_item(
    YR_OBJECT* object,
    int flags,
    int index)
{
  YR_OBJECT* result = NULL;
  YR_OBJECT_ARRAY* array;

  assert(object->type == OBJECT_TYPE_ARRAY);

  if (index < 0)
    return NULL;

  array = object_as_array(object);

  if (array->items != NULL && array->items->count > index)
      result = array->items->objects[index];

  if (result == NULL && flags & OBJECT_CREATE)
  {
    yr_object_copy(array->prototype_item, &result);

    if (result != NULL)
      yr_object_array_set_item(object, result, index);
  }

  return result;
}
