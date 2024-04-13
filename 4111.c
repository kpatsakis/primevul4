inline void GetMKLNodeDef(NodeDef* ndef) {
  // All MKL eager ops have `_kernel` private attribute that needs to be set
  // to a fixed label.
  AttrValue attr_kernel;
  attr_kernel.set_s(mkl_op_registry::kMklNameChangeOpLabel);
  (*ndef->mutable_attr()).insert({"_kernel", attr_kernel});
}