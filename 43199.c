void __init postgresql_init(void)
{
   dissect_add("postgresql", APP_LAYER_TCP, 5432, dissector_postgresql);
}
