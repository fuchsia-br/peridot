// Copyright 2017 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PERIDOT_LIB_FIREBASE_AUTH_FIREBASE_AUTH_IMPL_H_
#define PERIDOT_LIB_FIREBASE_AUTH_FIREBASE_AUTH_IMPL_H_

#include <functional>
#include <memory>
#include <string>

#include <lib/async/dispatcher.h>
#include <fuchsia/modular/auth/cpp/fidl.h>

#include "lib/backoff/backoff.h"
#include "lib/callback/cancellable.h"
#include "lib/callback/scoped_task_runner.h"
#include "peridot/lib/firebase_auth/firebase_auth.h"

namespace firebase_auth {

// Source of the auth information for cloud sync to use, implemented using the
// system token provider.
//
// If configured with an empty |api_key|, doesn't attempt to use
// |token_provider| and yields empty Firebase tokens and user ids. This allows
// the code to work without auth against public instances (e.g. for running
// benchmarks).
//
// *Warning*: if |token_provider| disconnects, all requests in progress are
// dropped on the floor. TODO(ppi): keep track of pending requests and call the
// callbacks with status TOKEN_PROVIDER_DISCONNECTED when this happens.
class FirebaseAuthImpl : public FirebaseAuth {
 public:
  FirebaseAuthImpl(async_t* async, std::string api_key,
                   fuchsia::modular::auth::TokenProviderPtr token_provider,
                   std::unique_ptr<backoff::Backoff> backoff);
  FirebaseAuthImpl(async_t* async, std::string api_key,
                   fuchsia::modular::auth::TokenProviderPtr token_provider,
                   std::unique_ptr<backoff::Backoff> backoff, int max_retries);

  // FirebaseAuth:
  void set_error_handler(fxl::Closure on_error) override;

  fxl::RefPtr<callback::Cancellable> GetFirebaseToken(
      std::function<void(firebase_auth::AuthStatus, std::string)> callback)
      override;

  fxl::RefPtr<callback::Cancellable> GetFirebaseUserId(
      std::function<void(firebase_auth::AuthStatus, std::string)> callback)
      override;

 private:
  // Retrieves the Firebase token from the token provider, transparently
  // retrying the request up to |max_retries| times in case of non-fatal
  // errors.
  void GetToken(int max_retries,
                std::function<void(firebase_auth::AuthStatus,
                                   fuchsia::modular::auth::FirebaseTokenPtr)>
                    callback);

  const std::string api_key_;
  fuchsia::modular::auth::TokenProviderPtr token_provider_;
  const std::unique_ptr<backoff::Backoff> backoff_;
  const int max_retries_;

  // Must be the last member field.
  callback::ScopedTaskRunner task_runner_;
};

}  // namespace firebase_auth

#endif  // PERIDOT_LIB_FIREBASE_AUTH_FIREBASE_AUTH_IMPL_H_
