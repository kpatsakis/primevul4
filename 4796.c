comp_addr_base (const void * v0, const void * v1)
{
  debug_info *info0 = *(debug_info **) v0;
  debug_info *info1 = *(debug_info **) v1;
  return info0->addr_base - info1->addr_base;
}