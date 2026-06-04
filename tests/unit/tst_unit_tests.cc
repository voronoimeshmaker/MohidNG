#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>

#include "MohidNG/Core/Error.h"
#include "MohidNG/Core/Logger.h"
#include "MohidNG/Fields/Field.h"
#include "MohidNG/IO/MeshReader.h"
#include "MohidNG/Numerics/Gradient.h"

namespace {

void Check(bool condition, const std::string& message) {
  if (!condition) {
    throw std::runtime_error(message);
  }
}

void ThrowInsideTraceForTest() {
  MOHIDNG_TRACE_SCOPE("ThrowInsideTraceForTest");
  mohidng::Raise(mohidng::Error("core.requirement_failed", "trace test"));
}

void TestLocalisedErrors() {
  mohidng::SetMessageLanguage("pt-br");
  Check(mohidng::GetMessageLanguage() == "pt-br", "Default language selection failed.");
  const mohidng::Error pt_br_error("mesh.file_not_found", "mesh.mngmesh");
  Check(pt_br_error.Message().find("Arquivo") != std::string::npos, "pt-br message failed.");

  mohidng::SetMessageLanguage("pt-pt");
  const mohidng::Error pt_pt_error("mesh.file_not_found", "mesh.mngmesh");
  Check(pt_pt_error.Message().find("Ficheiro") != std::string::npos, "pt-pt message failed.");

  mohidng::SetMessageLanguage("en-gb");
  const mohidng::Error en_gb_error("mesh.file_not_found", "mesh.mngmesh");
  Check(en_gb_error.Message().find("Mesh file") != std::string::npos, "en-gb message failed.");

  mohidng::SetMessageLanguage("unsupported-language");
  Check(mohidng::GetMessageLanguage() == "pt-br", "Unsupported language did not fall back to pt-br.");
}

void TestRegisteredError() {
  mohidng::RegisterError(mohidng::ErrorDefinition{
      "test.flexible_error",
      {{"pt-br", "Mensagem brasileira."}, {"pt-pt", "Mensagem portuguesa."}, {"en-gb", "British message."}}});

  mohidng::SetMessageLanguage("en-gb");
  Check(mohidng::Error("test.flexible_error").Message() == "British message.",
        "Registered flexible error failed.");
}

void TestExceptionCarriesTrace() {
  bool caught = false;
  try {
    ThrowInsideTraceForTest();
  } catch (const mohidng::MohidNgException& exception) {
    caught = true;
    Check(exception.GetError().Code() == "core.requirement_failed", "Exception error code failed.");
#ifndef NDEBUG
    Check(!exception.Trace().empty(), "Debug exception trace is empty.");
    Check(exception.TraceAsText().find("ThrowInsideTraceForTest") != std::string::npos,
          "Debug exception trace does not contain scope name.");
#else
    Check(exception.Trace().empty(), "Release exception trace should be empty.");
#endif
  }
  Check(caught, "Expected exception was not caught.");
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected mesh path argument.");
  TestLocalisedErrors();
  TestRegisteredError();
  TestExceptionCarriesTrace();

  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  Check(mesh.Cells().size() == 4, "Unexpected cell count.");
  Check(mesh.Faces().size() == 12, "Unexpected face count.");
  Check(mesh.BoundaryPatches().size() == 4, "Unexpected patch count.");

  mohidng::ScalarField constant("constant", mohidng::FieldLocation::kCell, mesh.Cells().size());
  for (std::size_t i = 0; i < constant.Size(); ++i) {
    constant[i] = 3.0;
  }
  const auto grad_constant = mohidng::ComputeCellGradient(mesh, constant);
  for (std::size_t i = 0; i < grad_constant.Size(); ++i) {
    Check(std::abs(grad_constant[i].x) < 1.0e-12, "Non-zero x-gradient for constant field.");
    Check(std::abs(grad_constant[i].y) < 1.0e-12, "Non-zero y-gradient for constant field.");
  }
  return 0;
}
