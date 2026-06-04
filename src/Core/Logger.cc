#include <sstream>
#include <utility>

#include <ostream>

#include <MohidNG/Core/Logger.h>


namespace mohidng {

#ifndef NDEBUG
namespace {
thread_local std::vector<std::string> trace_stack;
bool trace_logging_enabled = false;
std::ostream* trace_output = nullptr;

std::string MakeTraceEntry(std::string_view scope, const std::source_location& location) {
  std::ostringstream out;
  out << scope << " [" << location.file_name() << ':' << location.line() << ']';
  return out.str();
}
}  // namespace
#endif

TraceScope::TraceScope(std::string_view scope, const std::source_location& location) {
#ifndef NDEBUG
  entry_ = MakeTraceEntry(scope, location);
  trace_stack.push_back(entry_);
  if (trace_logging_enabled && trace_output != nullptr) {
    (*trace_output) << ">> " << entry_ << '\n';
  }
#else
  (void)scope;
  (void)location;
#endif
}

TraceScope::~TraceScope() {
#ifndef NDEBUG
  if (trace_logging_enabled && trace_output != nullptr) {
    (*trace_output) << "<< " << entry_ << '\n';
  }
  if (!trace_stack.empty()) {
    trace_stack.pop_back();
  }
#endif
}

std::vector<std::string> CurrentTrace() {
#ifndef NDEBUG
  return trace_stack;
#else
  return {};
#endif
}

std::string CurrentTraceAsText() {
  const auto trace = CurrentTrace();
  std::ostringstream out;
  for (const auto& entry : trace) {
    out << entry << '\n';
  }
  return out.str();
}

void ClearTrace() {
#ifndef NDEBUG
  trace_stack.clear();
#endif
}

std::string MakeTraceScopeName(std::string_view class_id, std::string_view method) {
  std::string name(class_id);
  if (!method.empty()) {
    name += "::";
    name += method;
  }
  return name;
}

void SetTraceLoggingEnabled(bool enabled) {
#ifndef NDEBUG
  trace_logging_enabled = enabled;
#else
  (void)enabled;
#endif
}

bool TraceLoggingEnabled() {
#ifndef NDEBUG
  return trace_logging_enabled;
#else
  return false;
#endif
}

void SetTraceOutput(std::ostream* output) {
#ifndef NDEBUG
  trace_output = output;
#else
  (void)output;
#endif
}

}  // namespace mohidng
