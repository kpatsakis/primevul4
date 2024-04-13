int yr_object_dict_set_item(
    YR_OBJECT* object,
    YR_OBJECT* item,
    const char* key)
{
  YR_OBJECT_DICTIONARY* dict;

  int i;
  int count;

  assert(object->type == OBJECT_TYPE_DICTIONARY);

  dict = object_as_dictionary(object);

  if (dict->items == NULL)
  {
    count = 64;

    dict->items = (YR_DICTIONARY_ITEMS*) yr_malloc(
        sizeof(YR_DICTIONARY_ITEMS) + count * sizeof(dict->items->objects[0]));

    if (dict->items == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    memset(dict->items->objects, 0, count * sizeof(dict->items->objects[0]));

    dict->items->free = count;
    dict->items->used = 0;
  }
  else if (dict->items->free == 0)
  {
    count = dict->items->used * 2;
    dict->items = (YR_DICTIONARY_ITEMS*) yr_realloc(
        dict->items,
        sizeof(YR_DICTIONARY_ITEMS) + count * sizeof(dict->items->objects[0]));

    if (dict->items == NULL)
      return ERROR_INSUFFICIENT_MEMORY;

    for (i = dict->items->used; i < count; i++)
    {
      dict->items->objects[i].key = NULL;
      dict->items->objects[i].obj = NULL;
    }

    dict->items->free = dict->items->used;
  }

  item->parent = object;

  dict->items->objects[dict->items->used].key = yr_strdup(key);
  dict->items->objects[dict->items->used].obj = item;

  dict->items->used++;
  dict->items->free--;

  return ERROR_SUCCESS;
}
