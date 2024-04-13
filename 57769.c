static inline void VectorSubtract3(const DDSVector3 left,
  const DDSVector3 right, DDSVector3 *destination)
{
  destination->x = left.x - right.x;
  destination->y = left.y - right.y;
  destination->z = left.z - right.z;
}
