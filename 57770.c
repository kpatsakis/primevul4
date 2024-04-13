static inline void VectorTruncate(DDSVector4 *value)
{
  value->x = value->x > 0.0f ? floor(value->x) : ceil(value->x);
  value->y = value->y > 0.0f ? floor(value->y) : ceil(value->y);
  value->z = value->z > 0.0f ? floor(value->z) : ceil(value->z);
  value->w = value->w > 0.0f ? floor(value->w) : ceil(value->w);
}
