static inline void VectorReciprocal(const DDSVector4 value,
  DDSVector4 *destination)
{
  destination->x = 1.0f / value.x;
  destination->y = 1.0f / value.y;
  destination->z = 1.0f / value.z;
  destination->w = 1.0f / value.w;
}
