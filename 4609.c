  Status AsGraphDefInternal(SerializationContext* ctx,
                            DatasetGraphDefBuilder* b,
                            Node** output) const override {
    Node* indices_node;
    TF_RETURN_IF_ERROR(b->AddTensor(sparse_tensor_.indices(), &indices_node));
    Node* value_node;
    TF_RETURN_IF_ERROR(b->AddTensor(sparse_tensor_.values(), &value_node));
    Node* dense_shape_node;
    std::vector<int64> dense_shape;
    dense_shape.reserve(sparse_tensor_.shape().size());
    for (int i = 0; i < sparse_tensor_.shape().size(); i++)
      dense_shape.emplace_back(sparse_tensor_.shape()[i]);
    TF_RETURN_IF_ERROR(b->AddVector(dense_shape, &dense_shape_node));
    AttrValue val_dtype;
    b->BuildAttrValue(sparse_tensor_.dtype(), &val_dtype);
    TF_RETURN_IF_ERROR(
        b->AddDataset(this, {indices_node, value_node, dense_shape_node},
                      {{"Tvalues", val_dtype}}, output));
    return Status::OK();
  }