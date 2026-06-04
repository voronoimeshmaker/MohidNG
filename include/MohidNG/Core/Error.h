#pragma once

#include <exception>
#include <map>
#include <source_location>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

#include <MohidNG/Core/ID.h>


namespace mohidng {

inline constexpr std::string_view kLanguagePtBr = "pt-br";
inline constexpr std::string_view kLanguagePtPt = "pt-pt";
inline constexpr std::string_view kLanguageEnGb = "en-gb";

struct ErrorDefinition {
  std::string code;
  std::map<std::string, std::string> messages;
};

struct ErrorContext {
  ClassIdentity identity{};
  std::source_location location = std::source_location::current();
};

[[nodiscard]] bool HasClassIdentity(const ErrorContext& context);
[[nodiscard]] ErrorContext MakeErrorContext(
    const std::source_location& location = std::source_location::current());
[[nodiscard]] ErrorContext MakeErrorContext(
    ClassIdentity identity, const std::source_location& location = std::source_location::current());

class Error {
 public:
  DefineIdentity("MohidNG.Core.Error")

  explicit Error(std::string code, std::string detail = {},
                 const std::source_location& location = std::source_location::current());
  Error(std::string code, std::string detail, ErrorContext context);

  const std::string& Code() const noexcept;
  const std::string& Detail() const noexcept;
  const ErrorContext& Context() const noexcept;
  std::string Message() const;
  std::string Message(std::string_view language) const;

 private:
  std::string code_;
  std::string detail_;
  ErrorContext context_;
};

class MohidNgException : public std::runtime_error {
 public:
  DefineIdentity("MohidNG.Core.MohidNgException")

  explicit MohidNgException(Error error);

  const Error& GetError() const noexcept;
  const std::vector<std::string>& Trace() const noexcept;
  std::string TraceAsText() const;

 private:
  static std::string BuildMessage(const Error& error, const std::vector<std::string>& trace);

  Error error_;
  std::vector<std::string> trace_;
};

void SetMessageLanguage(std::string language);
const std::string& GetMessageLanguage();
bool IsSupportedLanguage(std::string_view language);
void RegisterError(ErrorDefinition definition);
std::string ErrorMessage(std::string_view code, std::string_view language);

[[noreturn]] void Raise(Error error);
void Require(bool condition, const Error& error);
void Require(bool condition, std::string code, std::string detail = {},
             const std::source_location& location = std::source_location::current());
void Require(bool condition, std::string code, std::string detail, ErrorContext context);
void RequireMessage(bool condition, std::string detail,
                    const std::source_location& location = std::source_location::current());

}  // namespace mohidng

#define RequireClass(class_type, condition, code, detail) \
  ::mohidng::Require(condition, code, detail, \
                     ::mohidng::MakeErrorContext(class_type::kIdentity, std::source_location::current()))
