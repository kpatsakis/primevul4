static inline void VectorAdd(const DDSVector4 left, const DDSVector4 right,
  DDSVector4 *destination)
{
  destination->x = left.x + right.x;
  destination->y = left.y + right.y;
  destination->z = left.z + right.z;
  destination->w = left.w + right.w;
}
