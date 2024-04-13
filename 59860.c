count_established_intro_points(const rend_service_t *service)
{
  unsigned int num = 0;

  SMARTLIST_FOREACH(service->intro_nodes, rend_intro_point_t *, intro,
    num += intro->circuit_established
  );
  return num;
}
