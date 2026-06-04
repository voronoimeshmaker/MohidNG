#include "MohidNG/Core/Error.h"

#include <mutex>
#include <sstream>
#include <utility>

#include "MohidNG/Core/Logger.h"

namespace mohidng {
namespace {

std::mutex& RegistryMutex() {
  static std::mutex mutex;
  return mutex;
}

std::map<std::string, ErrorDefinition>& Registry() {
  static std::map<std::string, ErrorDefinition> registry;
  return registry;
}

std::string& SelectedLanguage() {
  static std::string language{kLanguagePtBr};
  return language;
}

void RegisterBuiltinErrors() {
  static bool registered = false;
  if (registered) {
    return;
  }
  registered = true;

  Registry().emplace("core.requirement_failed",
                     ErrorDefinition{"core.requirement_failed",
                                     {{std::string(kLanguagePtBr), "Requisito interno nao satisfeito."},
                                      {std::string(kLanguagePtPt), "Requisito interno nao satisfeito."},
                                      {std::string(kLanguageEnGb), "Internal requirement not satisfied."}}});
  Registry().emplace("mesh.file_not_found",
                     ErrorDefinition{"mesh.file_not_found",
                                     {{std::string(kLanguagePtBr), "Arquivo de malha nao encontrado."},
                                      {std::string(kLanguagePtPt), "Ficheiro de malha nao encontrado."},
                                      {std::string(kLanguageEnGb), "Mesh file not found."}}});
  Registry().emplace("mesh.parse_error",
                     ErrorDefinition{"mesh.parse_error",
                                     {{std::string(kLanguagePtBr), "Erro ao interpretar a malha."},
                                      {std::string(kLanguagePtPt), "Erro ao interpretar a malha."},
                                      {std::string(kLanguageEnGb), "Mesh parsing error."}}});
  Registry().emplace("mesh.invalid_connectivity",
                     ErrorDefinition{"mesh.invalid_connectivity",
                                     {{std::string(kLanguagePtBr), "Conectividade de malha invalida."},
                                      {std::string(kLanguagePtPt), "Conectividade de malha invalida."},
                                      {std::string(kLanguageEnGb), "Invalid mesh connectivity."}}});
  Registry().emplace("field.not_found",
                     ErrorDefinition{"field.not_found",
                                     {{std::string(kLanguagePtBr), "Campo escalar nao encontrado."},
                                      {std::string(kLanguagePtPt), "Campo escalar nao encontrado."},
                                      {std::string(kLanguageEnGb), "Scalar field not found."}}});
  Registry().emplace("field.invalid_size",
                     ErrorDefinition{"field.invalid_size",
                                     {{std::string(kLanguagePtBr), "Tamanho de campo incompativel."},
                                      {std::string(kLanguagePtPt), "Tamanho de campo incompativel."},
                                      {std::string(kLanguageEnGb), "Incompatible field size."}}});
  Registry().emplace("numerics.unsupported_gradient_method",
                     ErrorDefinition{"numerics.unsupported_gradient_method",
                                     {{std::string(kLanguagePtBr), "Metodo de gradiente nao suportado."},
                                      {std::string(kLanguagePtPt), "Metodo de gradiente nao suportado."},
                                      {std::string(kLanguageEnGb), "Unsupported gradient method."}}});
  Registry().emplace("numerics.gradient_requires_cell_field",
                     ErrorDefinition{"numerics.gradient_requires_cell_field",
                                     {{std::string(kLanguagePtBr), "O gradiente requer um campo definido em celulas."},
                                      {std::string(kLanguagePtPt), "O gradiente requer um campo definido em celulas."},
                                      {std::string(kLanguageEnGb), "Gradient reconstruction requires a cell field."}}});
  Registry().emplace("numerics.insufficient_neighbours",
                     ErrorDefinition{"numerics.insufficient_neighbours",
                                     {{std::string(kLanguagePtBr), "Vizinhos insuficientes para reconstruir o gradiente."},
                                      {std::string(kLanguagePtPt), "Vizinhos insuficientes para reconstruir o gradiente."},
                                      {std::string(kLanguageEnGb), "Insufficient neighbours for gradient reconstruction."}}});
  Registry().emplace("numerics.degenerate_geometry",
                     ErrorDefinition{"numerics.degenerate_geometry",
                                     {{std::string(kLanguagePtBr), "Geometria degenerada no operador numerico."},
                                      {std::string(kLanguagePtPt), "Geometria degenerada no operador numerico."},
                                      {std::string(kLanguageEnGb), "Degenerate geometry in numerical operator."}}});
  Registry().emplace("numerics.singular_least_squares",
                     ErrorDefinition{"numerics.singular_least_squares",
                                     {{std::string(kLanguagePtBr), "Sistema local de minimos quadrados singular."},
                                      {std::string(kLanguagePtPt), "Sistema local de minimos quadrados singular."},
                                      {std::string(kLanguageEnGb), "Singular local least-squares system."}}});
}

std::string DetailLabel(std::string_view language) {
  if (language == kLanguageEnGb) {
    return "Detail";
  }
  if (language == kLanguagePtPt) {
    return "Pormenor";
  }
  return "Detalhe";
}

std::string UnknownErrorMessage(std::string_view code, std::string_view language) {
  std::ostringstream out;
  if (language == kLanguageEnGb) {
    out << "Unregistered Mohid-NG error";
  } else if (language == kLanguagePtPt) {
    out << "Erro Mohid-NG nao registado";
  } else {
    out << "Erro Mohid-NG nao cadastrado";
  }
  out << " [" << code << "]";
  return out.str();
}

}  // namespace

Error::Error(std::string code, std::string detail) : code_(std::move(code)), detail_(std::move(detail)) {}

const std::string& Error::Code() const noexcept { return code_; }

const std::string& Error::Detail() const noexcept { return detail_; }

std::string Error::Message() const { return Message(GetMessageLanguage()); }

std::string Error::Message(std::string_view language) const {
  std::ostringstream out;
  out << ErrorMessage(code_, language);
  if (!detail_.empty()) {
    out << ' ' << DetailLabel(language) << ": " << detail_;
  }
  return out.str();
}

MohidNgException::MohidNgException(Error error)
    : std::runtime_error(BuildMessage(error, CurrentTrace())),
      error_(std::move(error)),
      trace_(CurrentTrace()) {}

const Error& MohidNgException::GetError() const noexcept { return error_; }

const std::vector<std::string>& MohidNgException::Trace() const noexcept { return trace_; }

std::string MohidNgException::TraceAsText() const {
  std::ostringstream out;
  for (const auto& entry : trace_) {
    out << entry << '\n';
  }
  return out.str();
}

std::string MohidNgException::BuildMessage(const Error& error, const std::vector<std::string>& trace) {
  std::ostringstream out;
  out << error.Message() << " [" << error.Code() << ']';
  if (!trace.empty()) {
    out << "\nTrace:\n";
    for (const auto& entry : trace) {
      out << "  - " << entry << '\n';
    }
  }
  return out.str();
}

void SetMessageLanguage(std::string language) {
  if (!IsSupportedLanguage(language)) {
    language = std::string(kLanguagePtBr);
  }
  std::lock_guard<std::mutex> lock(RegistryMutex());
  SelectedLanguage() = std::move(language);
}

const std::string& GetMessageLanguage() {
  std::lock_guard<std::mutex> lock(RegistryMutex());
  return SelectedLanguage();
}

bool IsSupportedLanguage(std::string_view language) {
  return language == kLanguagePtBr || language == kLanguagePtPt || language == kLanguageEnGb;
}

void RegisterError(ErrorDefinition definition) {
  std::lock_guard<std::mutex> lock(RegistryMutex());
  RegisterBuiltinErrors();
  Registry()[definition.code] = std::move(definition);
}

std::string ErrorMessage(std::string_view code, std::string_view language) {
  std::lock_guard<std::mutex> lock(RegistryMutex());
  RegisterBuiltinErrors();
  const auto& registry = Registry();
  const auto it = registry.find(std::string(code));
  if (it == registry.end()) {
    return UnknownErrorMessage(code, language);
  }
  const auto& messages = it->second.messages;
  const auto exact = messages.find(std::string(language));
  if (exact != messages.end()) {
    return exact->second;
  }
  const auto pt_br = messages.find(std::string(kLanguagePtBr));
  if (pt_br != messages.end()) {
    return pt_br->second;
  }
  const auto en_gb = messages.find(std::string(kLanguageEnGb));
  if (en_gb != messages.end()) {
    return en_gb->second;
  }
  return UnknownErrorMessage(code, language);
}

[[noreturn]] void Raise(Error error) { throw MohidNgException(std::move(error)); }

void Require(bool condition, const Error& error) {
  if (!condition) {
    Raise(error);
  }
}

void Require(bool condition, std::string code, std::string detail) {
  if (!condition) {
    Raise(Error(std::move(code), std::move(detail)));
  }
}

void RequireMessage(bool condition, std::string detail) {
  Require(condition, Error("core.requirement_failed", std::move(detail)));
}

}  // namespace mohidng
