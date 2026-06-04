#include <filesystem>
#include <mutex>
#include <sstream>
#include <utility>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>


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
  Registry().emplace("cli.invalid_arguments",
                     ErrorDefinition{"cli.invalid_arguments",
                                     {{std::string(kLanguagePtBr), "Argumentos de linha de comando invalidos."},
                                      {std::string(kLanguagePtPt), "Argumentos de linha de comando invalidos."},
                                      {std::string(kLanguageEnGb), "Invalid command-line arguments."}}});
  Registry().emplace("case.file_not_found",
                     ErrorDefinition{"case.file_not_found",
                                     {{std::string(kLanguagePtBr), "Arquivo de caso nao encontrado."},
                                      {std::string(kLanguagePtPt), "Ficheiro de caso nao encontrado."},
                                      {std::string(kLanguageEnGb), "Case file not found."}}});
  Registry().emplace("case.invalid_yaml",
                     ErrorDefinition{"case.invalid_yaml",
                                     {{std::string(kLanguagePtBr), "Arquivo YAML de caso invalido."},
                                      {std::string(kLanguagePtPt), "Ficheiro YAML de caso invalido."},
                                      {std::string(kLanguageEnGb), "Invalid YAML case file."}}});
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
  Registry().emplace("field.invalid_name",
                     ErrorDefinition{"field.invalid_name",
                                     {{std::string(kLanguagePtBr), "Nome de campo invalido."},
                                      {std::string(kLanguagePtPt), "Nome de campo invalido."},
                                      {std::string(kLanguageEnGb), "Invalid field name."}}});
  Registry().emplace("field.duplicate_name",
                     ErrorDefinition{"field.duplicate_name",
                                     {{std::string(kLanguagePtBr), "Nome de campo duplicado."},
                                      {std::string(kLanguagePtPt), "Nome de campo duplicado."},
                                      {std::string(kLanguageEnGb), "Duplicate field name."}}});
  Registry().emplace("field.invalid_location",
                     ErrorDefinition{"field.invalid_location",
                                     {{std::string(kLanguagePtBr), "Localizacao de campo incompativel."},
                                      {std::string(kLanguagePtPt), "Localizacao de campo incompativel."},
                                      {std::string(kLanguageEnGb), "Incompatible field location."}}});
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

const ErrorDefinition& DetailLabelDefinition() {
  static const ErrorDefinition definition{
      "core.detail_label",
      {{std::string(kLanguagePtBr), "Detalhe"},
       {std::string(kLanguagePtPt), "Pormenor"},
       {std::string(kLanguageEnGb), "Detail"}}};
  return definition;
}

const ErrorDefinition& UnknownErrorDefinition() {
  static const ErrorDefinition definition{
      "core.unregistered_error",
      {{std::string(kLanguagePtBr), "Erro Mohid-NG nao cadastrado"},
       {std::string(kLanguagePtPt), "Erro Mohid-NG nao registado"},
       {std::string(kLanguageEnGb), "Unregistered Mohid-NG error"}}};
  return definition;
}

std::string MessageFromDefinition(const ErrorDefinition& definition, std::string_view language) {
  const auto exact = definition.messages.find(std::string(language));
  if (exact != definition.messages.end()) {
    return exact->second;
  }
  const auto pt_br = definition.messages.find(std::string(kLanguagePtBr));
  if (pt_br != definition.messages.end()) {
    return pt_br->second;
  }
  const auto en_gb = definition.messages.find(std::string(kLanguageEnGb));
  if (en_gb != definition.messages.end()) {
    return en_gb->second;
  }
  return definition.code;
}

std::string DetailLabel(std::string_view language) {
  return MessageFromDefinition(DetailLabelDefinition(), language);
}

std::string UnknownErrorMessage(std::string_view code, std::string_view language) {
  std::ostringstream out;
  out << MessageFromDefinition(UnknownErrorDefinition(), language) << " [" << code << "]";
  return out.str();
}

}  // namespace

bool HasClassIdentity(const ErrorContext& context) { return !context.identity.id.empty(); }

ErrorContext MakeErrorContext(const std::source_location& location) {
  ErrorContext context;
  context.location = location;
  return context;
}

ErrorContext MakeErrorContext(ClassIdentity identity, const std::source_location& location) {
  ErrorContext context;
  context.identity = identity;
  context.location = location;
  return context;
}

Error::Error(std::string code, std::string detail, const std::source_location& location)
    : Error(std::move(code), std::move(detail), MakeErrorContext(location)) {}

Error::Error(std::string code, std::string detail, ErrorContext context)
    : code_(std::move(code)), detail_(std::move(detail)), context_(context) {}

const std::string& Error::Code() const noexcept { return code_; }

const std::string& Error::Detail() const noexcept { return detail_; }

const ErrorContext& Error::Context() const noexcept { return context_; }

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

  const auto& context = error.Context();
  const std::filesystem::path source_path(context.location.file_name());
  out << "\nContext:\n";
  if (HasClassIdentity(context)) {
    out << "  - class: " << context.identity.id << '\n';
  }
  out << "  - folder: " << source_path.parent_path().string() << '\n';
  out << "  - file: " << source_path.filename().string() << '\n';
  out << "  - line: " << context.location.line() << '\n';
  out << "  - function: " << context.location.function_name() << '\n';

  if (!trace.empty()) {
    out << "Trace:\n";
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
  const auto message = MessageFromDefinition(it->second, language);
  if (!message.empty()) {
    return message;
  }
  return UnknownErrorMessage(code, language);
}

[[noreturn]] void Raise(Error error) { throw MohidNgException(std::move(error)); }

void Require(bool condition, const Error& error) {
  if (!condition) {
    Raise(error);
  }
}

void Require(bool condition, std::string code, std::string detail, const std::source_location& location) {
  if (!condition) {
    Raise(Error(std::move(code), std::move(detail), location));
  }
}

void Require(bool condition, std::string code, std::string detail, ErrorContext context) {
  if (!condition) {
    Raise(Error(std::move(code), std::move(detail), context));
  }
}

void RequireMessage(bool condition, std::string detail, const std::source_location& location) {
  Require(condition, "core.requirement_failed", std::move(detail), location);
}

}  // namespace mohidng
