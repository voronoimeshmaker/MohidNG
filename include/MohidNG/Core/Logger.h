#pragma once

#include <string>
#include <string_view>
#include <vector>

#include <iosfwd>
#include <source_location>

#include <MohidNG/Core/ID.h>


namespace mohidng {

class TraceScope {
 public:
  DefineIdentity("MohidNG.Core.TraceScope")

  explicit TraceScope(std::string_view scope,
                      const std::source_location& location = std::source_location::current());
  TraceScope(const TraceScope&) = delete;
  TraceScope& operator=(const TraceScope&) = delete;
  TraceScope(TraceScope&&) = delete;
  TraceScope& operator=(TraceScope&&) = delete;
  ~TraceScope();

 private:
#ifndef NDEBUG
  std::string entry_;
#endif
};

std::vector<std::string> CurrentTrace();
std::string CurrentTraceAsText();
void ClearTrace();
std::string MakeTraceScopeName(std::string_view class_id, std::string_view method);
void SetTraceLoggingEnabled(bool enabled);
bool TraceLoggingEnabled();
void SetTraceOutput(std::ostream* output);

}  // namespace mohidng

#ifndef NDEBUG
#define MOHIDNG_CONCAT_IMPL(a, b) a##b
#define MOHIDNG_CONCAT(a, b) MOHIDNG_CONCAT_IMPL(a, b)
#define MOHIDNG_TRACE_SCOPE(name) \
  ::mohidng::TraceScope MOHIDNG_CONCAT(mohidng_trace_scope_, __LINE__)(name)
#define MOHIDNG_TRACE_CLASS(class_id, method) \
  ::mohidng::TraceScope MOHIDNG_CONCAT(mohidng_trace_scope_, __LINE__)(::mohidng::MakeTraceScopeName(class_id, method))
#else
#define MOHIDNG_TRACE_SCOPE(name) \
  do {                           \
  } while (false)
#define MOHIDNG_TRACE_CLASS(class_id, method) \
  do {                                      \
  } while (false)
#endif
