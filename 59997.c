guard_selection_infer_type(guard_selection_type_t type,
                           const char *name)
{
  if (type == GS_TYPE_INFER) {
    if (!strcmp(name, "bridges"))
      type = GS_TYPE_BRIDGE;
    else if (!strcmp(name, "restricted"))
      type = GS_TYPE_RESTRICTED;
    else
      type = GS_TYPE_NORMAL;
  }
  return type;
}
