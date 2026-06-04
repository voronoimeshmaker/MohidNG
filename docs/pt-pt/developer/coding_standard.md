# Norma de cÃƒÂ³digo

A norma de cÃƒÂ³digo do Mohid-NG adopta C++ moderno e convenÃƒÂ§ÃƒÂµes compatÃƒÂ­veis com Google Style quando apropriado, sem sacrificar clareza cientÃƒÂ­fica ou desempenho. As regras especÃƒÂ­ficas do projecto prevalecem sobre regras genÃƒÂ©ricas quando isso melhora a clareza numÃƒÂ©rica ou cientÃƒÂ­fica.

## Ficheiros fonte

```text
extensÃƒÂµes .h e .cc
header guards ou #pragma once nos cabeÃƒÂ§alhos
PascalCase para tipos
PascalCase para funÃƒÂ§ÃƒÂµes pÃƒÂºblicas quando se adopta Google Style
snake_case para variÃƒÂ¡veis
membros de classe terminam com underscore
constantes usam kNome
```

## Identificadores de classe

Toda a classe deve cadastrar um identificador estavel para mensagens de debug e erro. Use uma declaracao local na seccao publica da classe:

```cpp
class FieldSet {
 public:
  DefineIdentity("MohidNG.Fields.FieldSet")
};
```

Nao use enums centrais, RTTI ou nomes de tipo dependentes do compilador. Verificacoes pertencentes a uma classe devem usar `RequireClass(...)`, para que as excepcao reportem identificador da classe, pasta do fonte, ficheiro, linha e funcao.

## Directivas include

As directivas `#include` fazem parte do estilo do projecto e devem seguir esta sequÃƒÂªncia:

1. CabeÃƒÂ§alhos da biblioteca padrÃƒÂ£o C++, por ordem alfabÃƒÂ©tica.
2. CabeÃƒÂ§alhos de bibliotecas externas, por ordem alfabÃƒÂ©tica, como HDF5, PETSc, Trilinos ou outras dependÃƒÂªncias de terceiros.
3. CabeÃƒÂ§alhos do Mohid-NG, por ordem alfabÃƒÂ©tica.

Separe os trÃƒÂªs grupos de includes com uma linha em branco. Os cabeÃƒÂ§alhos do Mohid-NG devem usar sempre sinais de menor/maior, por exemplo:

```cpp
#include <cmath>
#include <string>
#include <vector>

#include <hdf5.h>
#include <petsc.h>

#include <MohidNG/Core/Error.h>
#include <MohidNG/Mesh/MeshView.h>
#include <MohidNG/Numerics/Gradient/Gradient.h>
```

NÃƒÂ£o use includes com aspas para cabeÃƒÂ§alhos do Mohid-NG.
## ComentÃƒÂ¡rios

Use comentÃƒÂ¡rios `//` nos ficheiros fonte. NÃƒÂ£o use comentÃƒÂ¡rios de bloco `/* ... */` no cÃƒÂ³digo do Mohid-NG. Para cabeÃƒÂ§alhos explicativos longos, como a introduÃƒÂ§ÃƒÂ£o dos exemplos, inicie e termine o bloco com uma linha separadora `//` com no mÃƒÂ¡ximo 80 colunas.
