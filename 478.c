  virtual bool ethernet_connecting() const {
    return ethernet_ ? ethernet_->connecting() : false;
  }
