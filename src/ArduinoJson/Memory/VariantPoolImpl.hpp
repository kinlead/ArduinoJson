// ArduinoJson - https://arduinojson.org
// Copyright © 2014-2023, Benoit BLANCHON
// MIT License

#pragma once

#include <ArduinoJson/Memory/VariantPool.hpp>
#include <ArduinoJson/Variant/VariantSlot.hpp>

ARDUINOJSON_BEGIN_PRIVATE_NAMESPACE

inline void VariantPool::create(size_t cap, Allocator* allocator) {
  ARDUINOJSON_ASSERT(slots_ == nullptr);
  if (!cap)
    return;
  slots_ = reinterpret_cast<VariantSlot*>(allocator->allocate(cap));
  if (slots_) {
    capacity_ = bytesToSlots(cap);
    usage_ = 0;
  }
}

inline void VariantPool::destroy(Allocator* allocator) {
  if (slots_)
    allocator->deallocate(slots_);
  slots_ = nullptr;
  capacity_ = 0;
  usage_ = 0;
}

inline void VariantPool::shrinkToFit(Allocator* allocator) {
  slots_ = reinterpret_cast<VariantSlot*>(
      allocator->reallocate(slots_, slotsToBytes(usage_)));
  capacity_ = usage_;
}

inline SlotWithId VariantPool::allocSlot() {
  if (!slots_)
    return {};
  if (usage_ >= capacity_)
    return {};
  auto index = usage_++;
  auto slot = &slots_[index];
  return {new (slot) VariantSlot, SlotId(index)};
}

inline VariantSlot* VariantPool::getSlot(SlotId id) const {
  return id == NULL_SLOT ? nullptr : &slots_[id];
}

inline SlotCount VariantPool::usage() const {
  return usage_;
}

inline SlotCount VariantPool::capacity() const {
  return capacity_;
}

inline void VariantPool::clear() {
  usage_ = 0;
}

inline SlotCount VariantPool::bytesToSlots(size_t n) {
  return static_cast<SlotCount>(n / sizeof(VariantSlot));
}

inline size_t VariantPool::slotsToBytes(SlotCount n) {
  return n * sizeof(VariantSlot);
}

ARDUINOJSON_END_PRIVATE_NAMESPACE
