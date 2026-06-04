#include <cmath>
#include <filesystem>
#include <stdexcept>
#include <string>

#include <MohidNG/Core/Constants.h>
#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>
#include <MohidNG/Fields/Field.h>
#include <MohidNG/Fields/FieldSet.h>
#include <MohidNG/IO/MeshReader.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>

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

void TestUnregisteredErrorMessages() {
  mohidng::SetMessageLanguage("pt-br");
  const auto pt_br = mohidng::Error("test.unregistered", "payload").Message();
  Check(pt_br.find("Erro Mohid-NG nao cadastrado") != std::string::npos,
        "pt-br unregistered error message failed.");
  Check(pt_br.find("Detalhe: payload") != std::string::npos, "pt-br detail label failed.");

  mohidng::SetMessageLanguage("pt-pt");
  const auto pt_pt = mohidng::Error("test.unregistered", "payload").Message();
  Check(pt_pt.find("Erro Mohid-NG nao registado") != std::string::npos,
        "pt-pt unregistered error message failed.");
  Check(pt_pt.find("Pormenor: payload") != std::string::npos, "pt-pt detail label failed.");

  mohidng::SetMessageLanguage("en-gb");
  const auto en_gb = mohidng::Error("test.unregistered", "payload").Message();
  Check(en_gb.find("Unregistered Mohid-NG error") != std::string::npos,
        "en-gb unregistered error message failed.");
  Check(en_gb.find("Detail: payload") != std::string::npos, "en-gb detail label failed.");
}


void TestClassIds() {
  Check(mohidng::Error::ClassId() == "MohidNG.Core.Error", "Error class ID changed.");
  Check(mohidng::Error::ClassName() == "MohidNG.Core.Error", "Error class name changed.");
  Check(mohidng::MohidNgException::ClassId() == "MohidNG.Core.MohidNgException",
        "Exception class ID changed.");
  Check(mohidng::TraceScope::ClassId() == "MohidNG.Core.TraceScope", "TraceScope class ID changed.");
  Check(mohidng::FieldLocation::ClassId() == "MohidNG.Fields.FieldLocation",
        "FieldLocation class ID changed.");
  Check(mohidng::ScalarField::ClassId() == "MohidNG.Fields.Field", "Field class ID changed.");
  Check(mohidng::FieldSet::ClassId() == "MohidNG.Fields.FieldSet", "FieldSet class ID changed.");
  Check(mohidng::MeshView::ClassId() == "MohidNG.Mesh.MeshView", "MeshView class ID changed.");
  Check(mohidng::GradientWeightingPolicy::ClassId() ==
            "MohidNG.Numerics.Gradient.GradientWeightingPolicy",
        "GradientWeightingPolicy class ID changed.");
  Check(mohidng::CellGradientWorkspace::ClassId() ==
            "MohidNG.Numerics.Gradient.CellGradientWorkspace",
        "CellGradientWorkspace class ID changed.");
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

void TestFieldSet(const mohidng::MeshView& mesh) {
  mohidng::FieldSet fields;
  auto& eta = fields.AddCellScalar("eta", mesh, 1.5);
  Check(eta.Size() == mesh.Cells().size(), "Cell scalar helper created wrong size.");
  Check(eta.Location() == mohidng::CellFieldLocation(), "Cell scalar helper created wrong location.");
  Check(std::abs(eta[0] - 1.5) < mohidng::kDefaultTolerance, "Cell scalar helper did not initialise value.");

  auto& face_flux = fields.AddFaceScalar("face_flux", mesh);
  Check(face_flux.Size() == mesh.Faces().size(), "Face scalar helper created wrong size.");
  auto& node_depth = fields.AddNodeScalar("node_depth", mesh, 2.0);
  Check(node_depth.Size() == mesh.Nodes().size(), "Node scalar helper created wrong size.");

  fields.RequireScalar("eta", mohidng::CellFieldLocation(), mesh.Cells().size());
  const auto names = fields.ScalarNames();
  Check(names.size() == 3, "Unexpected number of scalar field names.");

  bool duplicate_caught = false;
  try {
    fields.AddCellScalar("eta", mesh);
  } catch (const mohidng::MohidNgException& exception) {
    duplicate_caught = exception.GetError().Code() == "field.duplicate_name";
    const std::string message = exception.what();
    Check(message.find("Context:") != std::string::npos, "Exception message has no context block.");
    Check(message.find("class: MohidNG.Fields.FieldSet") != std::string::npos,
          "Exception message has no FieldSet class ID.");
    Check(message.find("folder:") != std::string::npos, "Exception message has no source folder.");
    Check(message.find("file: FieldSet.cc") != std::string::npos,
          "Exception message has no source file.");
    Check(message.find("line:") != std::string::npos, "Exception message has no source line.");
#ifndef NDEBUG
    Check(exception.TraceAsText().find(std::string(mohidng::FieldSet::ClassId())) != std::string::npos,
          "FieldSet class ID was not included in debug trace.");
#endif
  }
  Check(duplicate_caught, "Duplicate field name was not rejected.");

  bool empty_name_caught = false;
  try {
    fields.AddCellScalar("", mesh);
  } catch (const mohidng::MohidNgException& exception) {
    empty_name_caught = exception.GetError().Code() == "field.invalid_name";
  }
  Check(empty_name_caught, "Empty field name was not rejected.");

  bool location_caught = false;
  try {
    fields.RequireScalar("eta", mohidng::FaceFieldLocation(), mesh.Faces().size());
  } catch (const mohidng::MohidNgException& exception) {
    location_caught = exception.GetError().Code() == "field.invalid_location";
  }
  Check(location_caught, "Invalid field location was not rejected.");

  const mohidng::FieldLocation edge_location("Edge");
  Check(edge_location == mohidng::FieldLocation("edge"),
        "Field location names were not normalised to lower case.");
  fields.AddScalar(mohidng::ScalarField("edge_metric", edge_location, mesh.Faces().size()));
  fields.RequireScalar("edge_metric", mohidng::FieldLocation("edge"), mesh.Faces().size());
  Check(fields.Scalar("edge_metric").Location().Name() == "edge",
        "Custom field location was not preserved.");

  bool empty_location_caught = false;
  try {
    (void)mohidng::FieldLocation("");
  } catch (const mohidng::MohidNgException& exception) {
    empty_location_caught = exception.GetError().Code() == "field.invalid_location";
  }
  Check(empty_location_caught, "Empty field location was not rejected.");
}

}  // namespace

int main(int argc, char** argv) {
  Check(argc == 2, "Expected mesh path argument.");
  TestLocalisedErrors();
  TestRegisteredError();
  TestUnregisteredErrorMessages();
  TestClassIds();
  TestExceptionCarriesTrace();

  const auto mesh = mohidng::ReadBootstrapMesh(std::filesystem::path(argv[1]));
  Check(mesh.Cells().size() == 4, "Unexpected cell count.");
  Check(mesh.Faces().size() == 12, "Unexpected face count.");
  Check(mesh.BoundaryPatches().size() == 4, "Unexpected patch count.");
  TestFieldSet(mesh);

  mohidng::ScalarField constant("constant", mohidng::CellFieldLocation(), mesh.Cells().size());
  for (std::size_t i = 0; i < constant.Size(); ++i) {
    constant[i] = 3.0;
  }
  const auto grad_constant = mohidng::ComputeCellGradient(mesh, constant);
  for (std::size_t i = 0; i < grad_constant.Size(); ++i) {
    Check(std::abs(grad_constant[i].x) < mohidng::kDefaultTolerance, "Non-zero x-gradient for constant field.");
    Check(std::abs(grad_constant[i].y) < mohidng::kDefaultTolerance, "Non-zero y-gradient for constant field.");
  }
  return 0;
}
