#pragma once

#include <aggregate_by_key.h>
#include <as_data_flow.h>
#include <as_vector.h>
#include <dir.h>
#include <drop_nullopt.h>
#include <filter.h>
#include <join.h>
#include <open_files.h>
#include <out.h>
#include <split.h>
#include <transform.h>
#include <write.h>
#include <split_expected.h>

#include <optional>

template <typename Key, typename Value>
struct KV {
  Key key;
  Value value;
};

template <typename Base, typename Joined>
struct JoinResult {
  Base base;
  std::optional<Joined> joined;
};
