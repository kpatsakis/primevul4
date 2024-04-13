YR_OBJECT* yr_object_dict_get_item(
    YR_OBJECT* object,
    int flags,
    const char* key)
{
  int i;

  YR_OBJECT* result = NULL;
  YR_OBJECT_DICTIONARY* dict;

  assert(object->type == OBJECT_TYPE_DICTIONARY);

  dict = object_as_dictionary(object);

  if (dict->items != NULL)
  {
    for (i = 0; i < dict->items->used; i++)
    {
      if (strcmp(dict->items->objects[i].key, key) == 0)
        result = dict->items->objects[i].obj;
    }
  }

  if (result == NULL && flags & OBJECT_CREATE)
  {
    yr_object_copy(dict->prototype_item, &result);

    if (result != NULL)
      yr_object_dict_set_item(object, result, key);
  }

  return result;
}
