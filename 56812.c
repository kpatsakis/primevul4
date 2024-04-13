YR_OBJECT* yr_object_get_root(
    YR_OBJECT* object)
{
  YR_OBJECT* o = object;

  while (o->parent != NULL)
    o = o->parent;

  return o;
}
