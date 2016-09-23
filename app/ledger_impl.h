// Copyright 2016 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef APPS_LEDGER_APP_LEDGER_IMPL_H_
#define APPS_LEDGER_APP_LEDGER_IMPL_H_

#include <memory>
#include <unordered_map>

#include "apps/ledger/api/ledger.mojom.h"
#include "apps/ledger/storage/public/application_storage.h"
#include "lib/ftl/macros.h"
#include "mojo/public/cpp/bindings/strong_binding.h"

namespace ledger {

class LedgerImpl : public Ledger {
 public:
  LedgerImpl(mojo::InterfaceRequest<Ledger> request,
             std::unique_ptr<storage::ApplicationStorage> storage);
  ~LedgerImpl() override;

 private:
  // Ledger:
  void GetRootPage(const GetRootPageCallback& callback) override;
  void GetPage(mojo::Array<uint8_t> id,
               const GetPageCallback& callback) override;
  void NewPage(const NewPageCallback& callback) override;
  void DeletePage(mojo::Array<uint8_t> id,
                  const DeletePageCallback& callback) override;

  void SetConflictResolverFactory(
      mojo::InterfaceHandle<ConflictResolverFactory> factory,
      const SetConflictResolverFactoryCallback& callback) override;

  mojo::StrongBinding<Ledger> binding_;
  std::unique_ptr<storage::ApplicationStorage> storage_;

  FTL_DISALLOW_COPY_AND_ASSIGN(LedgerImpl);
};
}

#endif  // APPS_LEDGER_APP_LEDGER_IMPL_H_
