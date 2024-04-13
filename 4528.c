  virtual void expectWriteNewSessionTicket() {
    server->setEarlyDataAppParamsFunctions(
        [](const folly::Optional<std::string>&, const Buf&) { return false; },
        []() -> Buf {
          // This function shouldn't be called
          EXPECT_TRUE(false);
          return nullptr;
        });
    EXPECT_CALL(*getFakeHandshakeLayer(), writeNewSessionTicket(_)).Times(0);
  }