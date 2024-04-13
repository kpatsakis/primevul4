  explicit FakeServerHandshake(
      QuicServerConnectionState& conn,
      std::shared_ptr<FizzServerQuicHandshakeContext> fizzContext,
      bool chloSync = false,
      bool cfinSync = false,
      folly::Optional<uint64_t> clientActiveConnectionIdLimit = folly::none)
      : FizzServerHandshake(&conn, std::move(fizzContext)),
        conn_(conn),
        chloSync_(chloSync),
        cfinSync_(cfinSync),
        clientActiveConnectionIdLimit_(clientActiveConnectionIdLimit) {}