/*
 * Copyright 2018 Google
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "Firestore/core/src/firebase/firestore/remote/buffered_writer.h"

#include <utility>

#include "Firestore/core/src/firebase/firestore/util/hard_assert.h"

namespace firebase {
namespace firestore {
namespace remote {

void BufferedWriter::DiscardUnstartedWrites() {
  queue_ = {};
}

void BufferedWriter::Enqueue(std::unique_ptr<GrpcOperation> write) {
  HARD_ASSERT(write, "Trying to enqueue a null write operation");
  queue_.push(std::move(write));
  TryWrite();
}

void BufferedWriter::TryWrite() {
  if (empty()) {
    return;
  }
  if (has_active_write_) {
    return;
  }

  has_active_write_ = true;
  GrpcOperation* write_operation = queue_.front().release();
  HARD_ASSERT(write_operation, "Trying to execute a null operation");
  queue_.pop();
  write_operation->Execute();
}

void BufferedWriter::DequeueNext() {
  has_active_write_ = false;
  TryWrite();
}

}  // namespace remote
}  // namespace firestore
}  // namespace firebase
