# PadrÃƒÂ£o de cÃƒÂ³digo

O padrÃƒÂ£o de cÃƒÂ³digo do Mohid-NG adota C++ moderno e convenÃƒÂ§ÃƒÂµes compatÃƒÂ­veis com Google Style quando apropriado, sem sacrificar clareza cientÃƒÂ­fica ou desempenho. As regras especÃƒÂ­ficas do projeto prevalecem sobre regras genÃƒÂ©ricas quando isso melhora a clareza numÃƒÂ©rica ou cientÃƒÂ­fica.

## Arquivos fonte

```text
extensÃƒÂµes .h e .cc
header guards ou #pragma once nos cabeÃƒÂ§alhos
PascalCase para tipos
PascalCase para funÃƒÂ§ÃƒÂµes pÃƒÂºblicas quando adotando Google Style
snake_case para variÃƒÂ¡veis
membros de classe terminam com underscore
constantes usam kNome
```

## Identificadores de classe

Toda classe deve cadastrar um identificador estavel para mensagens de debug e erro. Use uma declaracao local na secao publica da classe:

```cpp
class FieldSet {
 public:
  DefineIdentity("MohidNG.Fields.FieldSet")
};
```

Nao use enums centrais, RTTI ou nomes de tipo dependentes do compilador. Verificacoes pertencentes a uma classe devem usar `RequireClass(...)`, para que as excecoes reportem identificador da classe, pasta do fonte, arquivo, linha e funcao.

## Diretivas include

As diretivas `#include` fazem parte do estilo do projeto e devem seguir esta sequÃƒÂªncia:

1. CabeÃƒÂ§alhos da biblioteca padrÃƒÂ£o C++, em ordem alfabÃƒÂ©tica.
2. CabeÃƒÂ§alhos de bibliotecas externas, em ordem alfabÃƒÂ©tica, como HDF5, PETSc, Trilinos ou outras dependÃƒÂªncias de terceiros.
3. CabeÃƒÂ§alhos do Mohid-NG, em ordem alfabÃƒÂ©tica.

Separe os trÃƒÂªs grupos de includes com uma linha em branco. Os cabeÃƒÂ§alhos do Mohid-NG devem sempre usar sinais de menor/maior, por exemplo:

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

Use comentÃƒÂ¡rios `//` nos arquivos fonte. NÃƒÂ£o use comentÃƒÂ¡rios de bloco `/* ... */` no cÃƒÂ³digo do Mohid-NG. Para cabeÃƒÂ§alhos explicativos longos, como a introduÃƒÂ§ÃƒÂ£o dos exemplos, inicie e termine o bloco com uma linha separadora `//` com no mÃƒÂ¡ximo 80 colunas.
