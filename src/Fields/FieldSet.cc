#include <algorithm>
#include <utility>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Core/Logger.h>
#include <MohidNG/Core/Types.h>
#include <MohidNG/Fields/FieldSet.h>


namespace mohidng {
namespace {

void Fill(ScalarField& field, Real value) {
  for (auto& entry : field.Values()) {
    entry = value;
  }
}

}  // namespace

void FieldSet::AddScalar(ScalarField field) {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "AddScalar");
  (void)AddScalarAndReturn(std::move(field));
}

ScalarField& FieldSet::AddCellScalar(std::string name, const MeshView& mesh, Real initial_value) {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "AddCellScalar");
  auto& field = AddScalarAndReturn(ScalarField(std::move(name), CellFieldLocation(), mesh.Cells().size()));
  Fill(field, initial_value);
  return field;
}

ScalarField& FieldSet::AddFaceScalar(std::string name, const MeshView& mesh, Real initial_value) {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "AddFaceScalar");
  auto& field = AddScalarAndReturn(ScalarField(std::move(name), FaceFieldLocation(), mesh.Faces().size()));
  Fill(field, initial_value);
  return field;
}

ScalarField& FieldSet::AddNodeScalar(std::string name, const MeshView& mesh, Real initial_value) {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "AddNodeScalar");
  auto& field = AddScalarAndReturn(ScalarField(std::move(name), NodeFieldLocation(), mesh.Nodes().size()));
  Fill(field, initial_value);
  return field;
}

bool FieldSet::HasScalar(const std::string& name) const { return scalars_.contains(name); }

ScalarField& FieldSet::Scalar(const std::string& name) {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "Scalar");
  auto it = scalars_.find(name);
  RequireClass(FieldSet, it != scalars_.end(), "field.not_found", name);
  return it->second;
}

const ScalarField& FieldSet::Scalar(const std::string& name) const {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "Scalar const");
  auto it = scalars_.find(name);
  RequireClass(FieldSet, it != scalars_.end(), "field.not_found", name);
  return it->second;
}

std::vector<std::string> FieldSet::ScalarNames() const {
  std::vector<std::string> names;
  names.reserve(scalars_.size());
  for (const auto& [name, field] : scalars_) {
    (void)field;
    names.push_back(name);
  }
  std::ranges::sort(names);
  return names;
}

void FieldSet::RequireScalar(const std::string& name, FieldLocation location, std::size_t size) const {
  MOHIDNG_TRACE_CLASS(FieldSet::ClassId(), "RequireScalar");
  const auto& field = Scalar(name);
  RequireClass(FieldSet, field.Location() == location, "field.invalid_location",
               name + " expected=" + location.Name() + " actual=" + field.Location().Name());
  RequireClass(FieldSet, field.Size() == size, "field.invalid_size", name);
}

ScalarField& FieldSet::AddScalarAndReturn(ScalarField field) {
  RequireClass(FieldSet, !field.Name().empty(), "field.invalid_name",
               "Scalar field name cannot be empty.");
  RequireClass(FieldSet, !HasScalar(field.Name()), "field.duplicate_name", field.Name());
  const auto [it, inserted] = scalars_.emplace(field.Name(), std::move(field));
  RequireClass(FieldSet, inserted, "field.duplicate_name", it->first);
  return it->second;
}

}  // namespace mohidng
