# MOHID-NG v0.14: relatório de revisão

Data: 8 de setembro de 2026.

Objeto: auditoria da definição do sistema v0.14, comparação com o repositório MOHID-NG e o MOHID original, e análise adicional do CMake do VMM solicitada pelo responsável pelo projeto.

**Resultado: baseline aproveitável, com revisões necessárias antes da expansão da implementação. Recomenda-se adotar a organização modular do CMake do VMM, com ajustes, e não copiar integralmente suas opções. A licença escolhida pelo responsável nesta revisão é MPL-2.0.**

Esta entrega é exclusivamente documental. Não foram alterados código, CMake, licenças, planilha, instruções de entrada ou histórico Git; não foram feitos commits nem PRs. As alterações descritas abaixo são propostas para uma etapa posterior.

## 1. Resumo executivo

A v0.14 estabelece uma direção científica coerente: Volumes Finitos, malhas externas, políticas numéricas substituíveis, componentes abertos e verificação independente. Também preserva corretamente alternativas futuras de integração temporal, acoplamento e geometria dinâmica.

Os pontos que impedem tratá-la como especificação operacional encerrada são:

1. **Contrato VoroGradToolkit/VMM/MOHID-NG incompleto.** O provedor público de gradientes recebe um tipo concreto do VMM e utiliza polígonos, geradores e informações de faces que não estão integralmente disponíveis no pacote HDF5 atual do MOHID-NG. Encontrar o pacote pelo CMake não resolve essa diferença.
2. **Invalidação insuficiente.** A geração descrita principalmente em termos de topologia não cobre movimento geométrico sem alteração de conectividade. Gradientes, normais, volumes e coeficientes podem ficar obsoletos mesmo mantendo os mesmos IDs.
3. **Gates sem contratos executáveis completos.** Existem nomes de gates, mas faltam casos fechados, métricas, tolerâncias justificadas, condições de aplicabilidade e artefatos de aprovação. Os nove testes atuais passam, mas não equivalem à aprovação científica da v0.14.
4. **Gate de prontidão mistura decisões e implementação.** O Item 16 exige materializar parte da arquitetura antes de declarar prontidão para iniciá-la. O Apêndice D também apresenta arquivos de métodos ainda não selecionados.
5. **Histórico e nova baseline precisam de reconciliação explícita.** Os Blocos 0, 1 e 2 estão registrados como concluídos. O código atual possui gradientes próprios, enquanto a v0.14 determina consumo do VoroGradToolkit. A transição não deve apagar a validade histórica dos blocos anteriores.
6. **A licença já foi decidida e sua aplicação começou durante esta tarefa.** MPL-2.0 substitui a antiga preferência por Apache-2.0 para a evolução do projeto. Na conferência final, novos commits externos a esta revisão já atualizavam `LICENSE`, README e D-OPEN-11. Restam verificar a consistência dos demais avisos e o inventário das dependências; não há autorização para relabelar código de terceiros.

Sobre desempenho, a recomendação é começar com **Release otimizado, IPO/LTO verificado, perfil nativo opcional, sem fast-math global**, mantendo uma referência portátil e uma configuração de diagnóstico separadas. PGO deve vir depois de cargas representativas. Não há evidência experimental nesta auditoria que permita prometer um percentual de aceleração ou afirmar que uma combinação de flags será a mais rápida em todos os casos.

Há uma oportunidade algorítmica concreta: a construção atual dos stencils WLS chama uma busca de vizinhos que percorre todas as faces para cada célula. Esse custo merece prioridade de medição sobre ajustes pequenos de compilação, respeitando a futura substituição do provedor de gradientes.

## 2. Escopo, evidências e limitações

### 2.1 Fontes efetivamente examinadas

| Fonte | Identificação da evidência |
| --- | --- |
| Documento de entrada | `C:/Users/jf_va/Downloads/MOHID-NG_Definicao_do_Sistema_v0.14_codex_review.md`; transcrição estruturada examinada, incluindo Itens 1 a 16 e Apêndices A a D. |
| MOHID-NG local | `//wsl.localhost/Ubuntu-26.04-Test/home/jflavio/Programas/MohidNG`; branch `main`, inicialmente limpa. |
| MOHID-NG, commit auditado | `71bb09bdd5f55a0076332d6d1b066bd5af3a3201`, merge do PR de rastreabilidade. [Repositório nesse commit](https://github.com/voronoimeshmaker/MohidNG/tree/71bb09bdd5f55a0076332d6d1b066bd5af3a3201). |
| Baseline anterior à v0.14/P0 | Tag anotada `baseline/pre-v014-p0`, apontando para `87bb29897f89194f5576c06789a9d46053fe3c80`. |
| Planilha operacional | `planning/traceability/MOHID-NG_Traceability_Register.xlsx`, lida sem alteração por esta tarefa; recebeu atualização concorrente de licenciamento. |
| VMM local | `//wsl.localhost/Ubuntu-26.04-Test/home/jflavio/Programas/VMM`; HEAD `6da489e3be61bcdda4f6f4f528fdee328b44ef58`, **com alterações locais preexistentes**. O CMake analisado é o conteúdo da árvore de trabalho, não apenas esse commit. |
| Origem Git do VMM | [voronoimeshmaker/voronoimeshmaker](https://github.com/voronoimeshmaker/voronoimeshmaker). A versão local modificada não foi publicada por esta tarefa. |
| MOHID original | [Mohid-Water-Modelling-System/Mohid](https://github.com/Mohid-Water-Modelling-System/Mohid), commit `57167f94a870bce0a687b4cd97e19cab36f96b9e`. Código e documentação oficial consultados. |
| VoroGradToolkit público | [livro-mvf/VoroGradToolkit](https://github.com/livro-mvf/VoroGradToolkit), commit `f221140a3f32c069ac783f7425004dbead1414ee`. |
| Decisões posteriores ao documento | Solicitação de comparar o CMake do VMM; proibição de modificar código nesta etapa; escolha explícita de `MPL-2.0`. Essas mensagens prevalecem sobre recomendações anteriores incompatíveis. |

**Atualização constatada no encerramento:** o HEAD avançou, por trabalho concorrente, para `32e60af5341285ff0d08520fcc4c9371f043915b`. Os commits `4f5fb7d80dc2276957c1a61f3fed2975bde57b0b`, `e6678f6006d74b9797fc0a3607a62d0e8996b179` e `32e60af5341285ff0d08520fcc4c9371f043915b` adotam MPL-2.0, atualizam seu texto oficial e o README. A planilha já marca D-OPEN-11 como `Definida`. Também foram observadas alterações locais concorrentes de documentação/CI. Nada disso foi revertido ou editado por esta tarefa. A análise detalhada de código e os testes continuam vinculados ao snapshot inicial `71bb09...`; as novidades de licença foram incorporadas ao parecer, sem alegar uma segunda auditoria completa da árvore em edição.

Identificadores de integridade SHA-256 do material examinado:

| Arquivo | SHA-256 |
| --- | --- |
| Documento Markdown de entrada | `C2CAAE0C229D3878BD7461F9399686838409EC59783880A16C22417967D4F4FB` |
| Planilha de rastreabilidade, snapshot inicial | `9B92172AA7ED09C7F2EEBD3012BCA0E3B603BAA2B0D3D455E47823B7F6A80835` |
| Planilha de rastreabilidade, leitura de encerramento | `F3BDC4ED2B3A593B4169F4750AE033D3909D2B9DC49226053A191F99296C9D8F` |
| VMM: `CMakeLists.txt` local | `207b4db745dd6c670bc0975dd390c36bd0426c2e8f6558a6ee3fad4a72ebd369` |
| VMM: `cmake/ConfigCompiler.cmake` local | `f717bfeed2d2226dca57c3389bacf3dc3e8d1ca4c67ff8c10eb904d875776327` |
| VMM: `examples/CMakeLists.txt` local | `55814dcb64d95fdedf450abfc05f4f9397fcd704873f00cb98339dff88bae3cd` |
| VMM: `tests/CMakeLists.txt` local | `5134e78f70b78c54d4a261fef78bac4e6d8c9fa0067fa2525b659847e1851707` |

As referências a caminhos e linhas de MOHID-NG abaixo usam o commit auditado; as referências ao VMM usam a árvore local identificada acima. Inferências estáticas, resultados executados e propostas são distinguidos no texto.

### 2.2 Verificações realizadas

Foi configurada e compilada uma árvore Debug independente em `/tmp/mohidng-v014-build-debug-20260908`, sem utilizar ou modificar o build de trabalho do projeto.

| Verificação | Resultado | Limite da evidência |
| --- | --- | --- |
| Configuração CMake | Aprovada; CMake 4.2.3, GCC 15.2.0, WSL Ubuntu-26.04-Test. | Uma configuração Linux; não valida todos os compiladores anunciados. |
| Compilação Debug | Aprovada. | Não mede desempenho Release. |
| CTest | **9/9 aprovados**, aproximadamente 0,05 s no conjunto. | Cinco programas de exemplo e quatro executáveis de teste; não são nove gates científicos. |
| Verificação de includes | `tools/check_include_style.py` aprovado. | Não implementa integralmente o novo padrão lexical do Apêndice C. |
| CMake do VMM | Inspeção do código e de cache/flags já existentes. | Não foi reconfigurado, recompilado, instalado nem submetido a benchmark nesta tarefa. |

Os testes atuais usam verificações explícitas e exceções, não apenas `assert`; portanto, não se deve afirmar que desaparecem integralmente em Release. Entretanto, **Release não foi reexecutado nesta auditoria**. A compilação e os testes acima ocorreram antes da última solicitação de conclusão; nenhuma otimização foi aplicada.

### 2.3 O que não foi demonstrado

- Não houve benchmark comparativo O2/O3/LTO/native/PGO, nem execução científica com PETSc.
- Não houve avaliação completa do VMM, do CRIVO ou da qualidade numérica de todos os métodos do VoroGradToolkit. A análise adicional do VMM concentrou-se no build e nas dependências relevantes.
- Os arquivos internos `[I1]` v0.8 e `[I2]` `MOHID_cases_review.md/PDF` não foram encontrados nos locais consultados; `[I3]` não foi disponibilizado para auditoria. As afirmações sobre C02 a C19 que dependem desses documentos permanecem pendentes de confirmação.
- A consulta exata ao endereço OpenFOAM v14 indicado em `[R3]` não foi concluída. Isso não demonstra que o endereço ou a afirmação sejam falsos.
- Não se considerou a existência de um módulo, uma opção ou um teste como prova de precisão científica, desempenho ou aprovação de release.
- Não se inferiu que arquivos discutidos anteriormente em outra distribuição WSL estejam presentes no repositório atual.

## 3. Achados e tabela consolidada de mudanças

**Prioridades:** P0 bloqueia uma decisão estrutural, aceitação ou distribuição correspondente; P1 deve ser resolvido antes de ampliar o primeiro pipeline; P2 é melhoria de manutenção. P0 não significa implementar toda a capacidade futura agora.

Os IDs `REV-01` a `REV-26` são identificadores **deste relatório**, não novos IDs oficiais da planilha. Cada proposta inclui localização, problema, evidência, ação, tipo e impacto.

| ID / prioridade | Localização e problema | Evidência | Mudança proposta / tipo | Impacto esperado no repositório |
| --- | --- | --- | --- | --- |
| REV-01 / P0 | Itens 8.1, 16.2 e D.2/D.4 misturam prontidão conceitual com materialização de arquitetura e algoritmos ainda abertos. | P0-11/P0-13 exigem árvore e infraestrutura; D lista `explicit_rk`, `imex`, `upwind_flux`, `tvd_flux`; D-OPEN-01/02 continuam abertas. | Dividir prontidão para decisões de prontidão para kernels; marcar arquivos de métodos não escolhidos como candidatos, não entregas obrigatórias. Tipo: requisito/arquitetura. | Revisão do gate e do planejamento; evitar criação de abstrações e arquivos vazios sem consumidor definido. |
| REV-02 / P0 | F-002/F-034, D-023, Itens 7 e D.5 não fecham o contrato com VoroGradToolkit. | `GradientWorkspace2D.hpp` do provedor recebe `ClippedVoronoiDiagram2D`, usa `sites`, polígonos e arestas; o HDF5 0.1 local não possui toda essa informação. | Especificar dados, ownership, versão, adaptador e testes de contrato antes do link; manter a decisão de provedor dedicado. Tipo: ADR/arquitetura. | Evolução versionada do pacote/adaptador; não copiar headers externos para o núcleo. |
| REV-03 / P0 | D.3, D-024 e F-029/F-033 associam invalidação insuficientemente à mudança geométrica. | O workspace local guarda coeficientes; o externo guarda métricas e referência ao diagrama. Coordenadas podem mudar sem trocar conectividade. | Definir identidade da malha e revisão geométrica/topológica, ou uma revisão única para toda mutação relevante; documentar lifetime e invalidação dos views. Tipo: requisito/ADR. | Contrato de malha/campos/workspaces e testes de cache obsoleto; nenhuma implementação ALE exigida agora. |
| REV-04 / P0 | Itens 10, 13 e QS-* têm critérios de aprovação não operacionalizados. | Gates possuem nomes; planilha não vincula scripts, tolerâncias e evidências de aprovação; testes locais cobrem bootstrap. | Criar ficha executável por gate com domínio, dados, oráculo, normas, tolerâncias justificadas, aplicabilidade, saída e responsável. Tipo: V&V. | Casos e testes rastreáveis; impede declarar validação apenas com compilação ou gráfico. |
| REV-05 / P0 | F-027, D-OPEN-08 e roadmap não distinguem suficientemente primeiro exemplo de verificação, primeiro caso científico e v1.0. | Primeiro caso segue aberto; a conversa anterior definiu difusão estacionária inicial com um solver e um precondicionador. | Registrar esse caso como candidato a DIFF-01/linha de verificação, sem promovê-lo automaticamente ao caso científico de v1.0. Tipo: requisito/ADR. | Ficha de escopo e critérios de saída; manter a sequência de três programas previamente discutida como preparação, não capacidade implementada. |
| REV-06 / P0 | Item 15 precisa incorporar a decisão MPL-2.0 e a conformidade da distribuição. | Usuário escolheu MPL-2.0; snapshot inicial usava Apache-2.0; novos commits já atualizam LICENSE/README e D-OPEN-11. Headers VGT/VMM inspecionados indicam GPL v3. | Reconhecer a escolha e aplicação já registradas; concluir auditoria de titularidade/dependências e consistência dos avisos. Tipo: ADR/documental. | Consolidar documentação e critérios de distribuição; não recriar a decisão nem relicenciar terceiros automaticamente. |
| REV-07 / P0 | F-025 e Items 7/11/14 não definem o contrato de avaliação independente pelo CRIVO. | Há intenção e IDs, mas nenhum contrato versionado, pacote de exemplo ou avaliação arquivada no Git inspecionado. | Definir schema mínimo de submissão/resultado, hashes, métricas, status, versão do avaliador e independência do oráculo. Tipo: arquitetura/V&V. | Manifesto e artefatos interoperáveis; CRIVO continua fora do runtime do solver. |
| REV-08 / P1 | Histórico do Bloco 2 conflita com a política atual de gradientes externos. | `docs/en-gb/developer/startup_plan.md:73` permite reimplementação própria; código local contém WLS e Green-Gauss próprios. | ADR de transição que supersede a orientação antiga, preserva conclusões históricas e exige equivalência demonstrada antes da substituição. Tipo: ADR/rastreabilidade. | Migração futura do provedor; reaproveitamento dos testes existentes e registro da procedência. |
| REV-09 / P1 | D-004 e contrato de IDs externos versus índices internos. | Reader HDF5 exige ID igual à posição; `src/Mesh/MeshView.cc:29` indexa diretamente o vetor. | Separar ID externo persistente de índice denso local, ou declarar formalmente a limitação da versão 0.1; testar mapeamento recuperável. Tipo: requisito/arquitetura. | Reader, tipos e manifestos evoluem sem renumerar silenciosamente dados externos. |
| REV-10 / P1 | F-001/F-026: validação de malha ainda incompleta para a nova baseline. | `VoronoiMeshPackageReader.cc:185` aceita área/comprimento positivos sem exigir finitude, não rejeita vizinho igual ao owner, não restringe todo sentinela negativo a -1 nem verifica unicidade de todos os IDs de face. | Especificar invariantes e acrescentar fixtures para cada erro; distinguir arquivo inexistente, HDF5 inválido e contrato inválido. Tipo: implementação/V&V. | Fortalecimento do reader e catálogo de erros. Achado estático, sem alegar reprodução dinâmica de todos esses casos. |
| REV-11 / P1 | F-019/D-010 e extensões: metadados podem ser confundidos com garantia científica. | Registro local associa nome a builder e sobrescreve entradas; não existe matriz validada método/malha/contorno/integrador. | Distinguir capacidade declarada, restrições e combinações verificadas; rejeitar colisões de registro; registrar provedor e versão. Tipo: requisito/arquitetura. | Registro determinístico, diagnósticos de incompatibilidade e evidências por combinação suportada. |
| REV-12 / P1 | F-003/F-009/F-015/F-016 e modelo de estado deixam semântica física insuficiente. | `Field` guarda localização e valores; `FieldSet` valida tipo de localização e tamanho, não unidades, malha/revisão ou significado conservado. | Explicitar quantidade extensiva/intensiva, unidade, componente, suporte, malha, ownership e consistência de updates acoplados. Tipo: requisito/ADR. | Extensão dos contratos existentes; não impor normalização de frações mássicas a todo campo ambiental. |
| REV-13 / P1 | F-010/F-011, Item 10.3 e D-OPEN-10 não fecham restart. | Histórico/estágios são requisitos; testes de restart são mencionados, mas política e formato permanecem abertos. | Separar avanço temporal e capacidade de checkpoint/restart, com aplicabilidade por integrador; decidir estado que deve persistir. Tipo: requisito/ADR. | Contrato para tempo, estado, geometria e reinício sem exigir implementação no caso estacionário. |
| REV-14 / P1 | F-023 e Item 14 citam formatos sem perfil operacional suficiente. | HDF5 local contém malha bootstrap; não há writer netCDF-CF/UGRID nem incidência completa para representar polígonos. | Fixar versão/perfil de saída, topologia, unidades, localização, CRS quando necessário, tempo e links aos IDs da malha. Tipo: requisito/arquitetura. | Schema e testes de leitura por ferramenta independente; VTK continua saída complementar útil. |
| REV-15 / P1 | F-028 a F-033 e D-016 a D-024 misturam capacidade futura com obrigação imediata. | D-020 declara CVT futura, mas F-031 está consolidado sem horizonte; suporte externo examinado é 2D. | Acrescentar aplicabilidade e marcos para superfície livre, camadas, 3D genérico, ALE e remapeamento; fechar semântica de extrusão versus Voronoi 3D. Tipo: requisito/roadmap. | Evita prometer 3D ou movimento antes dos contratos e gates dos provedores. |
| REV-16 / P1 | Item 11 e planilha: rastreabilidade já existe, mas não está integralmente operacional. | PR #1 criou registro; `03_Change_Register!E2` ainda diz pendente Git apesar de hash/PR preenchidos; AUD-0001 referencia CHG-0002 inexistente na aba de mudanças. | Reconciliar status e referências, reconhecer convenções de IDs já registradas e vincular gradualmente evidências reais. Tipo: rastreabilidade. | Atualização futura da planilha com revisão humana; não recriar IDs nem declarar P0-03 ausente. |
| REV-17 / P1 | Apêndice C versus padrão vigente e abertura numérica. | C.3 usa `MeshMotion` como exemplo de enum; padrão atual exige includes ordenados, `//`, identidade de classe e `Real`; C.8 usa `PetscReal`. | Remover enum de catálogo extensível; preservar as regras existentes ou registrar sua substituição; manter tipos próprios fora do adaptador PETSc. Tipo: documental/ADR. | Migração lexical separada da migração científica; manutenção da identidade de diagnóstico e aliases necessários. |
| REV-18 / P1 | Item 6 e CMake: copiar flags do VMM não garante velocidade nem comparabilidade. | VMM força native nos exemplos, ativa cobertura pela presença de ferramenta e distribui opções de forma desigual entre biblioteca e consumidores. | Adotar perfis explícitos e auditáveis, IPO por target e benchmark sem instrumentação. Tipo: build/V&V. | CMake modular proposto na seção 8, sem alteração agora. |
| REV-19 / P1 | Item 6/D.3: custo de travessia e preparação de gradientes. | `MeshView.cc:37` percorre todas as faces; `WeightedLeastSquares.cc:40` chama a construção por célula e linha 55 consulta vizinhos. | Medir setup separado do kernel; considerar adjacência compacta pré-construída e reutilização por revisão da malha. Tipo: arquitetura/desempenho. | Potencial redução de complexidade da preparação; não investir indiscriminadamente em um kernel que será substituído pelo VGT. |
| REV-20 / P1 | Item 14: manifesto ainda não fecha reprodutibilidade de configuração e build. | Lista de versões/seeds existe; faltam contratos de conteúdo, defaults resolvidos e política de ponto flutuante/threads. | Registrar hashes dos insumos, configuração efetiva, flags, precisão, ISA, runtime, threads/processos e critério de equivalência. Tipo: requisito/V&V. | Artefato RunManifest versionado e testes de repetição por perfil. |
| REV-21 / P1 | QS-EXT-001 exige carregamento sem recompilar; Item 8.3 o condiciona à viabilidade. | Diferença textual e ABI ainda aberta em D-OPEN-06. | Definir dois níveis: extensão recompilada e plugin binário; limitar garantia sem recompilar à ABI/plataforma suportada. Tipo: requisito/ADR. | Testes de compatibilidade, falha segura e ownership na fronteira; sem implementar loader genérico prematuramente. |
| REV-22 / P2 | Item 10.1 usa validação científica para comparação analítica/manufaturada. | Solução exata/manufaturada verifica implementação e discretização; não demonstra adequação ao fenômeno físico. | Separar verificação de código/cálculo de validação com dados físicos e suas incertezas. Tipo: documental/V&V. | Nomes corretos em gates, relatórios e documentação pública. |
| REV-23 / P2 | Itens 4/5 repetem capacidades e misturam requisitos com tecnologias. | Pares F-008/D-006, F-011/D-008, F-034/D-023; GTest aparece em F-024. | Manter princípio científico separado de comportamento testável; mover escolha de framework e detalhes de API para padrões/ADRs, preservando IDs rastreáveis. Tipo: requisito/documental. | Menos divergência normativa, sem apagar referências históricas. |
| REV-24 / P2 | Itens 9/13 e referências usam evidência sem disponibilidade ou critérios de fechamento. | I1/I2/I3 não auditáveis nesta sessão; riscos sem responsáveis e gatilhos; URL `main` não fixa versão. | Arquivar fontes permitidas ou metadados verificáveis, pin de commits, dono/gatilho/mitigação/evidência de fechamento de risco. Tipo: rastreabilidade. | Matriz de capacidades do legado e riscos auditáveis, sem afirmar validação dos casos C02-C19. |
| REV-25 / P1 | C.3/D.2/D.5: migração `.cc/.h` para `.cpp/.hpp` não acompanha descoberta atual. | Globs locais de biblioteca, exemplos e testes reconhecem apenas `.cc`; nomes e argumentos de dois exemplos têm tratamento central específico. | Planejar extensão dupla transitória, um executável por entry point, metadados locais de entrada/dependência e detecção de colisões. Tipo: build/implementação. | Preserva descoberta automática, `exe_*`, `tst_*` e `make run_nome`, sem exigir editar a raiz a cada exemplo. |
| REV-26 / P2 | D.1/D.2 e P0-11 chamam de preservação uma árvore parcialmente planejada. | Só Core, Mesh, Fields, IO e Numerics estão implementados; `cmake/` já existe; Physics/Lagrangian/LinearAlgebra aparecem como direção futura. | Distinguir árvore existente de árvore-alvo e novas pastas de infraestrutura; não inferir que módulos planejados já funcionam. Tipo: documental. | Planejamento honesto e refatoração incremental, sem criar todos os diretórios apenas para satisfazer um desenho. |

### 3.1 Ajustes científicos que precisam acompanhar os achados

**Conservação.** Fluxos internos opostos são necessários, mas o critério global deve contabilizar fronteiras, fontes, volumes, quantidade conservada e erro de solução. Um fluxo com sinal correto não prova sozinho conservação ao longo do tempo. Isso deve ser especificado para o primeiro operador físico, sem reabrir retrospectivamente o escopo concluído do Bloco 2.

**Difusão inicial.** Para o exemplo estacionário previamente discutido, distinguir explicitamente:

- Resíduo de discretização avaliado na solução exata: `R_exact = A_h phi_exact - b_h`.
- Resíduo algébrico após a solução: `r_alg = A_h phi_num - b_h`.
- Erro da solução: `e = phi_num - phi_exact`.

O relatório do caso deve declarar se as linhas de `A_h` e `b_h` representam balanço integrado ou equação dividida pelo volume. As normas têm escala diferente nessas duas convenções. A identificação de `R_exact` com um erro de truncamento e a interpretação da ordem exigem essa definição, a discretização das fontes e o tratamento dos contornos.

Para a fórmula de erro local fornecida pelo responsável nesta conversa, preservar:

`e_norm[S,i] = abs(S_num[i] - S_exact[i]) / (abs(S_exact[i]) + epsilon_S)`

`epsilon_S = 1e-12 * max_j abs(S_exact[j])`.

Se a referência for identicamente zero, o denominador também será zero. A política para esse caso precisa ser decidida e testada, por exemplo erro absoluto ou indicador de não aplicabilidade. Não substituir silenciosamente a fórmula por outra tolerância. Identificar também se `S` é a variável de solução ou um termo fonte, para não conflitar com a notação do sistema linear.

**Verificação versus validação.** A distinção de REV-22 segue a prática de verificação de código por soluções analíticas/manufaturadas e estudos de refinamento, documentada pela [NASA/NPARC](https://www.grc.nasa.gov/www/wind/valid/tutorial/verassess.html). Comparações com observações físicas exigem uma camada adicional de validação e incertezas.

## 4. Comparação com código e histórico Git

### 4.1 Marcos existentes

| Marco | Evidência | Interpretação |
| --- | --- | --- |
| Bootstrap e Bloco 0 | Histórico inicial, licença Apache-2.0 e `startup_plan.md`. | Decisões históricas válidas para aquela baseline; a nova licença é uma mudança posterior. |
| Bloco 1 | Commit `1145315b049a526bae27321f7108e76fa906ea05`; reader HDF5, contrato bootstrap e fixtures. | Concluído no planejamento, em 4 de junho de 2026. |
| Bloco 2 | Commit `87bb29897f89194f5576c06789a9d46053fe3c80`; campos e gradientes; plano marca conclusão em 4 de junho de 2026. | Não contém ainda o pipeline físico PETSc da nova baseline. |
| Preservação pré-P0 | `baseline/pre-v014-p0` resolve para o commit do Bloco 2. | P0-01 já tem evidência; não criar uma segunda baseline equivalente sem motivo. |
| Rastreabilidade | Commits `04e8e72054f693b237cd7f0ad4e3755eb08d2d91`, `8ca5db7ef85a2bad26dcdbc8fc8528fe0f3ce362`, `2492dea5df090c724df0850b01ae1879347870a5` e merge `71bb09...`. | Entre baseline e HEAD, a alteração versionada é a planilha. Não houve nessa faixa implementação dos novos kernels. |

Não se recomenda marcar os blocos históricos como incompletos por requisitos introduzidos posteriormente. Recomenda-se registrar quais artefatos serão migrados ou substituídos sob a v0.14 e manter datas e critérios originais.

### 4.2 Classificação do acervo

| Artefato / localização | Classificação | Justificativa e condição |
| --- | --- | --- |
| `include/MohidNG/Core/Types.h` e `Constants.h` | REUSE | Tipos e constantes centralizados atendem às preferências do projeto. Validar conversões no adaptador PETSc, sem espalhar `PetscReal`. |
| `Core/ID.h`, erros, exceções e logging | REUSE | Identidade por classe e contexto de erro já existem. Adaptar nomenclatura apenas com política de compatibilidade e testes. |
| `Mesh/Index.h` | REUSE | IDs distintos de células/faces/nós são boa base. Completar contratos para IDs externos, patches e revisões. |
| `Mesh/MeshTypes.h`, `MeshView.h`, `src/Mesh/MeshView.cc` | REFACTOR | Base 2D útil; distinguir ownership/view, completar incidências, adjacências eficientes e revisão geométrica. |
| `Fields/Field.h` e `FieldSet` | REFACTOR | Localização textual normalizada é extensível. Faltam associação a malha, semântica física e estado multicomponente. |
| Reader HDF5 e contrato `mngpkg` 0.1 | REFACTOR | Reaproveitar RAII e validação existentes; evoluir esquema, invariantes e mapeamentos sem quebrar silenciosamente fixtures antigas. |
| Reader de malha textual bootstrap | REUSE | Útil para testes pequenos. Declarar seu papel, sem confundi-lo com contrato científico definitivo. |
| Registry/workspace de gradientes | REFACTOR | Seleção externa ao kernel e coeficientes pré-calculados são úteis. Acrescentar identidade/provedor/revisão e adaptar ao VGT. |
| `src/Numerics/Gradient/WeightedLeastSquares.cc` e `ClassicalGreenGauss.cc` | REPLACE, condicionado | A política nova pede provedor externo. Substituir somente depois do contrato, licenciamento e gates de equivalência; não remover nesta revisão. |
| Quatro executáveis `tst_*` existentes | REUSE | Preservar cenários e verificações; migrar a GTest de forma incremental caso mantida a decisão. Acrescentar testes científicos e negativos específicos. |
| Fixtures HDF5 permanentes | REUSE | Já existem casos inválidos para área negativa e owner inválido. Expandir, não recriar como se estivessem ausentes. |
| Exemplos Ex01 a Ex04 | REUSE | Linha didática para leitura, campos e gradientes; atualizar contratos quando necessário. |
| Ex05, domínio retangular com YAML em `.ini` | INVESTIGATE | Manter como exemplo pedagógico; esclarecer que sua geometria de demonstração não substitui a responsabilidade do VMM na cadeia científica de produção. |
| CMake raiz e três módulos atuais | REFACTOR | Já são target-based e têm autodiscovery; falta separar políticas, dependências, perfis e empacotamento. Não é uma reescrita do zero. |
| `.clang-format`, `.clang-tidy`, scripts e CI | REFACTOR | Aproveitar infraestrutura e estender para o padrão aprovado e perfis científicos. |
| Documentação trilíngue e plano dos blocos | REFACTOR | Preservar história e corrigir normas supersedidas, links e distinção entre existente/planejado. |
| Planilha de rastreabilidade | REFACTOR | Cadastro inicial já entregue; reconciliar status, ligações e evidências. |
| Physics, Lagrangian, LinearAlgebra e novos programas físicos | INVESTIGATE | Não há implementação desses módulos na árvore auditada; não atribuir-lhes classificação de código funcional. |
| Exclusões imediatas de código | REMOVE: nenhuma indicada | Não há justificativa para apagar agora código funcional. Remoções futuras devem ser consequência de substituição verificada ou de duplicação comprovada. |

## 5. MOHID original e fronteiras externas

### 5.1 O que o legado realmente sustenta

O repositório oficial contém uma base Fortran e uma separação extensa entre infraestrutura, hidrodinâmica, transporte, propriedades, processos locais e outras aplicações. O [README no commit consultado](https://github.com/Mohid-Water-Modelling-System/Mohid/blob/57167f94a870bce0a687b4cd97e19cab36f96b9e/README.md) situa MOHID Water e MOHID Land. Isso sustenta a necessidade de uma matriz de capacidades, não a obrigação de implementar todo o legado na primeira release.

| Evidência primária | Consequência para a revisão |
| --- | --- |
| [ModuleAdvectionDiffusion.F90](https://github.com/Mohid-Water-Modelling-System/Mohid/blob/57167f94a870bce0a687b4cd97e19cab36f96b9e/Software/MOHIDBase2/ModuleAdvectionDiffusion.F90), incluindo opções de esquemas e TVD. | A abertura a métodos alternativos é coerente com o legado. Não obriga herdar decomposição direcional ou implementação Fortran. |
| [ModuleInterface.F90](https://github.com/Mohid-Water-Modelling-System/Mohid/blob/57167f94a870bce0a687b4cd97e19cab36f96b9e/Software/MOHIDBase1/ModuleInterface.F90), propriedades, modelos locais e controle de passo. | Componentes/processos e subpassos merecem contratos separados. |
| [ModuleGeometry.F90](https://github.com/Mohid-Water-Modelling-System/Mohid/blob/57167f94a870bce0a687b4cd97e19cab36f96b9e/Software/MOHIDBase2/ModuleGeometry.F90), estruturas e opções de evolução geométrica. | A evolução do estado geométrico não deve ser descartada por uma primeira implementação fixa. |
| [Documentação oficial de hidrodinâmica](https://wiki.mohid.com/index.php?title=Module_Hydrodynamic), incluindo drying/flooding e opções numéricas. | Superfície livre e wet/dry pertencem ao patrimônio científico a preservar, com horizonte de implementação explícito. |

A presença de opções não demonstra que cada combinação seja apropriada a qualquer caso. Também não é possível reconstruir com segurança o mapeamento C02-C19 apenas a partir de nomes de diretórios de exemplos. Esse mapeamento exige a evidência interna I2 e a inspeção dos casos correspondentes.

### 5.2 Contrato VoroGradToolkit/VMM

No [workspace público do VGT](https://github.com/livro-mvf/VoroGradToolkit/blob/f221140a3f32c069ac783f7425004dbead1414ee/include/VoroGradToolkit/Mesh/GradientWorkspace2D.hpp), há dependência concreta do diagrama do VMM, retenção de referência e extração de dados de geradores, polígonos e faces. O CMake exporta um target utilizável; portanto, a questão não é ausência de `find_package`, mas suficiência e estabilidade do contrato.

Decisões necessárias antes do adaptador:

- Posição de armazenamento da incógnita: gerador, centroide, centro geométrico ou média de volume. Não são conceitos intercambiáveis em uma malha Voronoi genérica.
- Incidências necessárias: face-vértice, célula-face, orientação, faces internas/de contorno e mapeamento entre gerador e célula.
- Ownership do diagrama e duração de vida do workspace, inclusive após movimento, reordenação ou retesselação.
- Limites e capacidades por método, contorno, dimensionalidade e qualidade da malha.
- Método canônico do MOHID-NG versus identificador do provedor, sem renomear silenciosamente métodos externos.
- Versões de API, pacote de malha e dependências transitivas.

O README do provedor descreve uma fase inicial WLS; o código examinado contém mais formulações. A descrição da v0.14 deve citar um commit e inventariar capacidades verificadas, não congelar uma leitura antiga do README como catálogo completo.

Atribuição recomendada de responsabilidades:

| Componente | Responsabilidade | Não assumir implicitamente |
| --- | --- | --- |
| VMM | Produzir geometria/topologia e metadados; operações geométricas contratadas. | Que conhece as equações e as quantidades físicas a conservar. |
| VGT | Reconstruir gradientes segundo suas capacidades e entradas documentadas. | Que aceitará automaticamente o pacote HDF5 atual do MOHID-NG ou qualquer malha 3D. |
| MOHID-NG | Consumir e validar contratos; montar balanços, fontes/contornos, estado e saída; decidir transferência física conservativa. | Que precisa gerar novamente a malha recebida ou expor tipos do VMM/PETSc na API física. |
| CRIVO | Avaliar artefatos por critérios independentes e registrar resultados. | Que sua aprovação pode ser substituída pelo mesmo kernel que está sendo testado. |

Em remapeamento futuro, separar explicitamente o cálculo geométrico de interseções/pesos da semântica física de transferência. A definição atual não atribui suficientemente essa interface.

## 6. Licenciamento: decisão MPL-2.0

**Decisão do responsável, recebida nesta revisão: o MOHID-NG usará Mozilla Public License 2.0, identificador `MPL-2.0`.** Não é mais uma alternativa a escolher em D-OPEN-11. Na conferência final, o registro já consta como `Definida` e `LICENSE` já contém MPL-2.0, por commits realizados fora desta tarefa. A revisão não fez essas alterações. Permanecem a consolidação normativa e a verificação de conformidade das contribuições e dependências.

A MPL tem copyleft no nível de arquivo e admite composição com código sob outras licenças; isso não dispensa as obrigações desses componentes. A Mozilla também aceita identificação SPDX em comentário apropriado. A licença do projeto não transforma automaticamente código externo em MPL. [FAQ oficial da Mozilla, questões 1, 4, 13 e 14](https://www.mozilla.org/en-US/MPL/2.0/FAQ/).

A distribuição de fonte/executável e de uma obra maior está disciplinada nas seções 3.1 a 3.4. A via de combinação com licenças secundárias tem condições, inclusive não usar a marca de incompatibilidade do Exhibit B quando se pretende essa via. Não se deve inferir que escolher MPL elimina obrigações de uma dependência GPL ou permite redistribuir toda combinação apenas como MPL. [Texto oficial MPL-2.0](https://www.mozilla.org/en-US/MPL/2.0/).

| Componente | Evidência desta revisão | Providência proposta |
| --- | --- | --- |
| Código próprio MOHID-NG | Apache-2.0 no histórico e decisão atual MPL-2.0. | Verificar titularidade/contribuições; aplicar a decisão a uma nova baseline com avisos coerentes, preservando direitos e histórico anteriores. |
| PETSc | BSD-2-Clause segundo sua documentação; pacotes baixados têm licenças próprias. | Registrar versão e dependências opcionais efetivamente distribuídas. [Licença oficial PETSc](https://petsc.org/release/install/license/). |
| VoroGradToolkit | Headers inspecionados declaram GNU GPL v3; não foi encontrado arquivo LICENSE raiz versionado no commit público examinado. | Confirmar escopo, versão exata e permissões com o titular; resolver distribuição da combinação antes de integrá-la a uma release. |
| VMM | Header local `ClippedVoronoiDiagram2D.hpp:9` declara GNU GPL v3; nenhum LICENSE/COPYING foi retornado pelo inventário versionado consultado. | Confirmar licenciamento do acervo e dependências, inclusive a forma de consumo pelo VGT. |
| MOHID original | Headers inspecionados declaram GPL versão 2. | Usar como referência científica e funcional; qualquer reaproveitamento de código demanda análise específica, não simples troca de cabeçalho. |
| HDF5, yaml-cpp, TBB, CGAL e demais transitivas | Presentes no build atual ou no caminho VMM/VGT; inventário legal completo não realizado. | Registrar componente, versão, licença, forma de link/distribuição e avisos. `yaml-cpp` precisa constar no inventário atual do projeto. |
| Trilinos | Não identificado como dependência efetivamente ligada no MOHID-NG auditado. | Só concluir compatibilidade para os pacotes/versões selecionados, quando houver integração. |

Sequência documental proposta: vincular os commits já feitos a um ADR da decisão MPL-2.0; inventariar titularidade e terceiros; definir o conteúdo da distribuição; conferir coerência entre `LICENSE`, avisos, documentação e metadados; verificar a release. Não adicionar a marca Exhibit B por padrão, pois isso contrariaria a intenção de manter a via de compatibilidade secundária disponível.

Esta é uma avaliação técnica de conformidade, não um parecer jurídico. Combinações concretas com GPL e eventuais direitos de contribuintes requerem confirmação apropriada. A escolha da licença está resolvida; essas verificações de aplicação não a reabrem.

## 7. Avaliação dos Itens 1 a 16 e Apêndices A a D

| Parte | Avaliação | Ajuste necessário |
| --- | --- | --- |
| 1. Visão, missão, escopo e fronteiras | Coerente; preserva capacidades sem exigir cópia do legado. | Delimitar paridade de longo prazo versus v1.0 e incluir o VGT de forma explícita na representação das fronteiras. REV-02/05/24. |
| 2. Stakeholders, personas e casos | Útil como visão de uso. | Identificar quem aprova ciência, integração de dependências e release; não presumir dados disponíveis dos casos. REV-04/24. |
| 3. Modelo conceitual | Boa separação física/método e abertura de componentes. | Fechar significado de estado, fontes, quantidades conservadas, localização e revisões. REV-03/12/13. |
| 4. Requisitos funcionais | Cobertura ampla, mas todos consolidados não significa todos implementados. | Acrescentar aplicabilidade, aceite e disposição por requisito; separar capacidades de decisões tecnológicas. Seção 9. |
| 5. Requisitos científicos | FV, fluxo único e alternativas abertas são consistentes. | Especificar conservação completa, posições de incógnitas, 3D em camadas e contrato VGT. REV-02/09/15. |
| 6. Qualidade | Intenções corretas, medição insuficiente. | Perfis científicos, cargas e orçamento de regressão; matriz real de compiladores; resolver QS-EXT-001. REV-18/19/20/21. |
| 7. Fronteiras | Responsabilidades principais fazem sentido. | Contrato de dados/ownership com VGT e CRIVO; sem confundir validação do consumidor com geração geométrica. REV-02/07. |
| 8. Arquitetura | Composição e seleção fora de loops são apropriadas. | Não exigir registry/plugin universal antes de um consumidor; definir restrições de ABI e build científico. REV-01/11/18/21. |
| 9. Riscos | Catálogo útil, ainda qualitativo. | Dono, gatilho, mitigação e fechamento; incluir transição de licença, dependências transitivas, invalidação e benchmarks. REV-24. |
| 10. V&V | Gates relevantes, mas não executáveis por definição. | Fichas de aceite, oráculos independentes, refinamento, tolerâncias e terminologia. REV-04/22. |
| 11. Rastreabilidade | Boa cadeia conceitual; parte já operacionalizada em Git. | Sincronizar texto com planilha, status e referências; não gerar novos IDs concorrentes. REV-16. |
| 12. ADRs e decisões abertas | Separação útil entre princípio e escolha concreta. | Formalizar ADRs maduros; MPL-2.0 deixa de ser escolha aberta; manter algoritmos e ABI realmente abertos. Seção 11. |
| 13. Roadmap | Sequência 2D, camadas, 3D genérico é razoável. | Distinguir verificação de primeiro caso científico; auditar dependências de casos e suporte externo. REV-05/15/24. |
| 14. Reprodutibilidade | Lista boa, mas ainda sem schema verificável. | Manifesto de conteúdo e configuração resolvida, perfis de FP/threads, política de equivalência. REV-20. |
| 15. Licença/governança | Correta separação dos dois temas. | Incorporar decisão MPL-2.0 e transição; governança pós-1.0 permanece separada. REV-06. |
| 16. Prontidão | Não atendido integralmente no estado auditado. | Reconhecer P0-01 e P0-03 parcial; remover circularidade e dividir marcos. Seção 12. |
| Apêndice A | Consolidação útil como registro de decisões. | Distinguir decisão aceita, proposta, implementação e evidência; atualizar licença e referências supersedidas. |
| Apêndice B | Referências primárias pertinentes. | Fixar versões; atualizar inventário VGT; não usar disponibilidade de biblioteca como prova de capacidade do MOHID-NG. |
| Apêndice C | Direção de consistência válida. | Plano de migração lexical, sem enum de métodos, preservando tipos próprios, identidade, includes e comentários. REV-17/25. |
| Apêndice D | Útil como árvore-alvo candidata, excessivo como gate rígido. | Diferenciar existente/planejado, corrigir geração geométrica, adiar métodos não escolhidos, aproveitar CMake já existente. REV-01/03/18/25/26. |

## 8. CMake do VMM e proposta de desempenho para o MOHID-NG

### 8.1 Parecer

**Sim, o MOHID-NG pode adotar o mesmo princípio de organização:** raiz enxuta, módulos em `cmake/`, targets explícitos, opções com prefixo do projeto, dependências por targets importados e descoberta automática dos programas. Isso já está parcialmente presente no MOHID-NG.

Entretanto, organização de build e velocidade de execução são coisas distintas. Dividir o CMake em mais arquivos melhora manutenção, mas não torna um kernel mais rápido por si só. O ganho depende do código gerado, dados, memória, montagem, solver e carga real.

No VMM foram encontrados `ConfigOptions`, `ConfigCompiler`, `ConfigDependencies`, `ConfigTargets`, `ConfigExamples`, `ConfigTests`, `ConfigPaper`, `ConfigDocs` e `ConfigUtils`. Essa divisão é reaproveitável como conceito. Suas opções não devem ser copiadas sem os ajustes abaixo.

### 8.2 Achados específicos do VMM

Todos os itens desta tabela são propostas de build/V&V associadas a REV-18 e REV-25. Nenhuma alteração foi aplicada ao VMM.

| ID / prioridade | Localização / evidência | Problema e impacto | Proposta para o padrão MOHID-NG |
| --- | --- | --- | --- |
| CM-01 / P1 | `examples/CMakeLists.txt:129` aplica `-O3 -march=native` em Release sem consultar `VMM_ENABLE_NATIVE_ARCH`. | Desligar native na opção central não basta para produzir exemplos portáteis; também diverge de multi-config. | Uma única função/política de otimização, respeitada por todos os targets próprios e baseada em configuração, compilador e plataforma. |
| CM-02 / P1 | `tests/CMakeLists.txt:125` detecta gcovr e linhas 167-170 adicionam cobertura quando encontrado. | A presença de uma ferramenta altera os binários e distorce medições. Flags geradas já existentes mostram `--coverage -fprofile-arcs -ftest-coverage` em um teste Release. | Opção explícita de cobertura, desligada em perfis de desempenho; instrumentar coerentemente biblioteca e testes apenas no perfil correspondente. |
| CM-03 / P1 | `ConfigTargets.cmake:31` liga opções de projeto como PRIVATE; testes/paper não chamam consistentemente o helper de otimização. | Em bibliotecas com muitos templates/headers, código importante é compilado no consumidor. Otimizar apenas a biblioteca não uniformiza os kernels dos executáveis. | Separar requisitos públicos de uso das políticas privadas; aplicar a política de build também a exemplos, testes e benchmarks próprios. Não exportar native/warnings para todo consumidor externo. |
| CM-04 / P1 | `ConfigCompiler.cmake:55` separa sanitizers e IPO, mas usa variáveis globais de inicialização de IPO. | Boa separação diagnóstica, porém política global pode atingir targets que não deveriam receber a opção. | Usar `CheckIPOSupported` e propriedades IPO por target/configuração; desligar explicitamente nos perfis incompatíveis e registrar valor efetivo. |
| CM-05 / P1 | `ConfigOptions.cmake:45` liga native por padrão; `ConfigCompiler.cmake:30` oferece fast-math, inicialmente OFF. | Native reduz portabilidade; fast-math pode mudar propriedades aritméticas e invalidar verificações. | Native apenas em perfil local/CPU-alvo conhecido. Manter fast-math global OFF e nenhuma adoção automática de `-Ofast`. |
| CM-06 / P2 | `tests/CMakeLists.txt:79` descobre testes sem `CONFIGURE_DEPENDS`. | Novo arquivo pode não ser descoberto em um build incremental sem reconfigurar. | Preservar autodiscovery com regeneração testada; um target por `tst_*`, nomes únicos e diagnóstico de colisão. |
| CM-07 / P1 | `examples/CMakeLists.txt:69` agrega vários `ex_*.cpp` da mesma pasta em um executável. | Se cada programa tiver `main`, haverá conflito; dependência YAML inferida por texto do fonte é frágil. | Um entry point por `exe_*`; arquivos auxiliares/dependências/argumentos em metadados locais explícitos, sem editar CMake raiz. |
| CM-08 / P1 | Exemplos, linha 125, e paper, linha 76, põem binários junto ao fonte; runners usam caminho montado manualmente. | Mistura fonte/artefato e dificulta múltiplos perfis e executáveis com sufixo `.exe`. | Binários em cada build, execução por `$<TARGET_FILE:...>` e diretório de resultados por caso; dados de entrada podem continuar ao lado do código. |
| CM-09 / P2 | `ConfigOptions.cmake` mantém aliases antigos com `CACHE ... FORCE`; usa variáveis globais de layout e `BUILD_SHARED_LIBS`. | Um valor antigo em cache pode prevalecer sobre a nova opção e interferir quando consumido como subprojeto. | Migração de opções com precedência documentada; defaults apenas no projeto principal; não sobrescrever decisões do projeto consumidor. |
| CM-10 / P1 | `ConfigTargets.cmake:52` exporta a biblioteca; opções privadas não integram o export; config de pacote encontra CGAL/TBB, não ZLIB. | Caminho shared atual não prova que static e instalação relocável funcionam. No modo static, dependências privadas de link precisam ser resolvidas pelo pacote consumidor. | Testes separados de instalação/consumo static/shared; não exportar targets internos de flags desnecessariamente; declarar dependências realmente requeridas. Risco estático identificado, não falha executada aqui. |
| CM-11 / P2 | `tests/CMakeLists.txt:266` chama um alvo de sequencial, mas apenas lista dependências. | O nome não impede execução paralela pelo build; processos de teste e threads internas podem competir por CPU. | Execução sequencial efetiva por CTest com paralelismo definido; benchmarks isolados, orçamento de threads explícito. |
| CM-12 / P1 | `ConfigDocs.cmake:19` tenta instalar dependências Python na configuração; linha 161 remove a pasta publicável configurada. | Configurar documentação tem efeitos externos; uma pasta de publicação errada pode colocar conteúdo indevido em risco. | Ambiente de documentação preparado explicitamente; publicação em pasta gerada dedicada e validada sob `doc/`, sem remoção de árvore arbitrária. |

O cache existente do VMM informa Release, native ON, LTO ON, fast-math OFF, sanitizers OFF e gcovr presente. As flags geradas da biblioteca incluem `-O3`, `-flto=auto`, `-march=native` e `-frounding-math`; um teste inspecionado inclui cobertura e não inclui native. Esses arquivos gerados são evidência do build existente, **não prova de uma nova configuração consistente com toda a árvore modificada**.

### 8.3 Organização proposta, ainda não implementada

Manter `CMakeLists.txt` na raiz e a lógica reutilizável em `cmake/`, preservando a preferência do projeto. Uma decomposição possível, ajustável antes da implementação:

```text
CMakeLists.txt
cmake/
  project/
    MohidNGOptions.cmake
    MohidNGCompilerOptions.cmake
    MohidNGTargets.cmake
    MohidNGPerformance.cmake
  dependencies/
    MohidNGDependencies.cmake
  testing/
    MohidNGExamples.cmake
    MohidNGTests.cmake
    MohidNGBenchmarks.cmake
  package/
    MohidNGConfig.cmake.in
    MohidNGInstall.cmake
  docs/
    MohidNGDocs.cmake
```

É uma proposta de responsabilidade, não uma exigência de criar todos os arquivos vazios. `Benchmarks` e empacotamento podem entrar quando houver o primeiro consumidor. Dependências ainda não utilizadas não devem virar obrigatórias apenas porque aparecem no roadmap.

Separar pelo menos três responsabilidades de target: requisitos públicos de compilação/API, warnings de desenvolvimento e otimizações dos targets próprios. Para biblioteca estática, `PRIVATE` não elimina necessidades de link do consumidor; validar o pacote instalado. Usar opções/propriedades por target, não editar globalmente `CMAKE_CXX_FLAGS`.

Presets versionados podem registrar perfis reproduzíveis. Se adotados, o descritor `CMakePresets.json` normalmente fica na raiz e pode referenciar descrições sob `cmake/`; isso deve ser uma exceção explícita à preferência de concentrar a lógica nessa pasta. Não criar esse arquivo nesta fase. Validar a versão mínima do CMake para o schema escolhido, inclusive se forem usados includes de presets. [Documentação de presets do CMake](https://cmake.org/cmake/help/latest/manual/cmake-presets.7.html).

**Descoberta automática:** manter `exe_nome`, `tst_nome` e `make run_nome`; oferecer `run_exe_nome` apenas como alias de compatibilidade se aprovado. Durante eventual migração de extensão, reconhecer `.cc` e `.cpp`, sem criar dois targets para o mesmo programa. Dados como `RectangularDomainGradient.ini` continuam junto ao código. A identificação de entrada e dependências deve ser local ao exemplo, não uma lista central crescente.

O CMake alerta para limitações de globs na manutenção de fontes. Neste projeto a autodiscovery é uma preferência explícita: pode ser mantida com `CONFIGURE_DEPENDS`, ordenação, convenções e testes de adição/remoção de arquivos nos geradores suportados. Isso não exige que o usuário edite a raiz a cada fonte novo. [Documentação de `file(GLOB)`](https://cmake.org/cmake/help/latest/command/file.html#glob).

**Documentação web:** preservar a publicação sob `doc/`, fora de `build`, conforme orientação anterior. Não transferir executáveis para `doc/` e não limpar toda essa pasta para publicar HTML.

### 8.4 Perfis de compilação recomendados

Todos os nomes abaixo são propostas, ainda inexistentes no build. O objetivo é máxima velocidade **para uma precisão e um caso definidos**, não maximizar agressividade das flags.

| Perfil proposto | Otimização e CPU | Instrumentação | Uso / critério |
| --- | --- | --- | --- |
| Diagnóstico | Debug; sem native; IPO desligado. | ASan/UBSan quando suportados, símbolos e testes de contratos. | Detectar erros; nunca usar seu tempo como velocidade de produção. |
| Referência portátil | Release; começar com O3 do toolchain e comparar O2; ISA de distribuição definida; IPO inicialmente OFF para a comparação. | Sem cobertura/sanitizers. | Referência científica e de desempenho; não promete igualdade bit a bit entre arquiteturas. |
| Release portátil com IPO | Mesmo perfil anterior, IPO ON somente se suportado e aprovado nos testes. | Sem instrumentação. | Avaliar otimização entre unidades sem perder portabilidade de CPU escolhida. |
| Release local | Release, IPO validado, native ON em compilador/plataforma suportados. | Sem instrumentação. | Candidato principal à execução nesta máquina; não distribuir como binário universal. |
| Profiling | RelWithDebInfo, nível de otimização explicitado, CPU igual à comparação; frame pointers quando úteis à ferramenta. | Apenas o necessário ao profiler. | Localizar custo real; medir o binário final novamente sem instrumentação adicional. |
| Cobertura | Configuração própria, preferencialmente sem IPO, biblioteca e testes instrumentados. | Coverage explicitamente ON. | Medir cobertura, não desempenho. Não ativar pela simples presença do gcovr. |
| PGO | Perfil de treino e depois perfil de uso, com mesma toolchain/flags compatíveis e corpus identificado. | Contadores só na fase de treino. | Entrar após benchmarks representativos; validar em casos que não participaram do treino. |

O3, IPO e PGO são hipóteses de otimização, não garantias de aceleração. O GCC documenta transformações adicionais por nível e mecanismos de otimização baseada em perfil; a escolha deve ser confirmada com medição do programa. [Opções de otimização GCC](https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html). O suporte a IPO deve ser testado pelo CMake, incluindo o ambiente efetivo de compilação/link. [CheckIPOSupported](https://cmake.org/cmake/help/latest/module/CheckIPOSupported.html).

`-march=native` permite instruções da CPU detectada e pode impedir execução em outra CPU. Não aplicar esse sinalizador GCC a MSVC, arquiteturas diferentes ou compilação cruzada sem uma política específica. O perfil portátil deve ter sua arquitetura-alvo definida. [GCC, opções x86](https://gcc.gnu.org/onlinedocs/gcc/x86-Options.html).

### 8.5 O que não recomendar como padrão

- Não ligar `-ffast-math` ou `-Ofast` globalmente. Não remover tratamento de NaN/Inf, signed zero ou arredondamento apenas para obter um número melhor no benchmark. Compiladores podem assumir ausência de NaN/Inf e reassociar operações nesse modo. [Clang, semântica de fast-math](https://clang.llvm.org/docs/UsersManual.html#cmdoption-ffast-math).
- Não chamar opções de ponto flutuante de seguras apenas pelo nome do cache. O VGT público exporta opções como `-fno-signed-zeros` em seu perfil chamado `FAST_MATH_SAFE`; elas precisam entrar na análise do executável inteiro.
- Não sobrepor flags de geometria indiscriminadamente. O build existente do VMM importa `-frounding-math`; qualquer relaxamento deve ser confrontado com os contratos da versão de CGAL efetivamente utilizada.
- Não impor unrolling manual, fast reciprocal, AVX específico ou flags agressivas a todos os fontes sem evidência. Code size, memória e comportamento numérico podem piorar.
- Não desabilitar o gerenciador de erros, validação de entrada ou exceções necessárias para acelerar o código. Validar contratos fora do laço crítico quando possível e preservar falhas inequívocas.
- Não tornar toda dependência PUBLIC para propagar otimizações; isso altera consumidores externos e pode contaminar bibliotecas de terceiros.
- Não confundir `make -j`, Ninja, ccache, precompiled headers ou unity build com aceleração do programa. São ferramentas de tempo de compilação; PCH/unity exigem cuidado com dependências e isolamento de unidades.

### 8.6 Prioridade das otimizações do código, após autorização

1. **Medir separadamente leitura, preparação geométrica/stencils, montagem, solução e saída.** Um tempo total não identifica o componente responsável.
2. **Adjacências e caches:** avaliar substituir a varredura completa de faces por adjacência compacta construída uma vez. Hoje, `Nc` consultas sobre `Nf` faces custam ordem `Nc*Nf`; uma lista de incidências permite construção proporcional às entidades e consultas proporcionais aos vizinhos. Essa é análise de complexidade, não speedup medido.
3. **Reutilização:** a malha fixa permite reutilizar workspaces e, quando o problema permitir, padrão de esparsidade e estrutura do precondicionador. Invalidar a cada revisão geométrica/topológica pertinente; não reutilizar coeficientes quando parâmetros físicos mudarem.
4. **Layout e alocações:** medir alocações por stencil, dispersão de dados e locality. Considerar arrays compactos/offsets e spans no caminho crítico, sem impor SoA universal antes do perfil.
5. **PETSc, quando integrado:** medir montagem, pré-alocação esparsa, custo de setup e iterações; usar build otimizado das dependências e observar `-log_view`. Flags do MOHID-NG não recompilam automaticamente PETSc/BLAS/HDF5. Essas práticas são coerentes com o [guia de desempenho PETSc](https://petsc.org/release/manual/performance/).
6. **Serialidade explícita:** um processo MPI não implica uma thread. VGT/VMM possuem usos de `std::execution::par_unseq`; BLAS/TBB também podem usar threads. Começar com um orçamento de execução controlado e registrar a configuração de cada backend.
7. **PGO e vetorizações específicas:** somente depois de saber onde está o custo e de existir um conjunto de casos que represente produção.

A escolha do solver/precondicionador pode afetar mais o tempo total do que flags, mas não será ampliada nesta fase contra o acordo anterior de um solver e um precondicionador no primeiro caso. Primeiro medir essa configuração fixa; comparações de métodos pertencem a uma etapa posterior aprovada.

### 8.7 Protocolo de medição proposto

Usar malhas VMM armazenadas e identificadas por hash, campos definidos e precisão-alvo constante. A geração da malha fica fora da medição do solver; pode ter benchmark próprio.

| Medida | Como comparar | Critério proposto |
| --- | --- | --- |
| Setup de malha/workspace | Mesma malha, método e ordem dos dados; tamanhos crescentes. | Identificar custo assintótico e memória; separar custo único de custo repetido. |
| Kernel de gradiente | Repetições suficientes para superar ruído, mantendo uso observável dos resultados. | Mesmos erros/normas aceitos e nenhuma alocação inesperada no caminho repetido. |
| Montagem/solução de difusão | Mesmo sistema, tolerâncias, solver, precondicionador e critérios de parada. | Tempo, iterações, resíduos algébricos e erro de solução, não apenas wall time. |
| End-to-end | Mesma saída científica e frequência de escrita. | Confirmar que uma melhoria local melhora a execução completa. |
| Regressão científica | Referência portátil versus cada perfil otimizado. | Tolerâncias e invariantes aprovados por caso, não igualdade cega de arquivos binários. |
| Comparação estatística | Aquecimento, repetições, mediana e dispersão; ambiente e afinidade registrados. | Ganho maior que o ruído e sem regressão relevante em outro caso; limiar final a definir com dados. |

Registrar CPU, memória, WSL/kernel, compilador/linker, flags efetivas, bibliotecas, threads/processos, configuração, hashes, pico de memória e versão do código. Não executar benchmarks concorrentes com compilação ou testes pesados. Usar WSL como ambiente de trabalho declarado, não extrapolar seus números automaticamente para Linux nativo ou Windows/MSVC.

**Ordem recomendada para experimentar:** Release portátil → IPO → native local → otimização do gargalo observado → PGO. Cada passo deve poder ser desligado e comparado isoladamente. A migração organizacional do CMake deve ser verificada primeiro sem mudar o perfil numérico, para separar regressão de build de mudança de código gerado.

## 9. Disposição dos requisitos

Manter um requisito significa preservar sua intenção, não declarar implementação concluída. Reescrever deve conservar o identificador; ao dividir ou mover, registrar sucessores e relações na planilha, sem apagar a história.

### 9.1 Requisitos funcionais

| ID | Disposição | Ajuste / critério |
| --- | --- | --- |
| REQ-F-001 | Manter e completar | Schema versionado, invariantes de entrada e fixtures negativas específicas. REV-02/10. |
| REQ-F-002 | Manter | Independência de implementação geométrica; fronteira explícita do adaptador VGT. REV-02. |
| REQ-F-003 | Reescrever | Localização + malha/revisão + significado/quantidade de entidades. REV-12. |
| REQ-F-004 | Manter | Configuração física deve rejeitar opções incompatíveis; esquema do caso a definir. |
| REQ-F-005 | Manter | Especificar unidades, suporte e precedência entre função, arquivo e configuração. |
| REQ-F-006 | Manter | Faces reais e cobertura completa dos patches; ausência de ghost físico não proíbe halo MPI futuro. |
| REQ-F-007 | Reescrever | Operadores necessários ao caso selecionado, com contrato de balanço e aceite; não catálogo ilimitado imediato. |
| REQ-F-008 | Manter | Fluxo interno único e contribuições opostas; integrar fontes/fronteiras no gate de conservação. |
| REQ-F-009 | Reescrever | Distinguir taxa volumétrica e contribuição integrada, unidades e quantidade conservada. REV-12. |
| REQ-F-010 | Dividir | Avanço/histórico/estágios versus persistência/restart, com aplicabilidade transiente. REV-13. |
| REQ-F-011 | Manter | Substituição do integrador; método inicial continua D-OPEN-01. |
| REQ-F-012 | Manter | Advecção separada da física; primeiro esquema ainda não congelado. |
| REQ-F-013 | Manter e completar | Reconstrução/limitador com restrições de malha e contorno e combinações verificadas. |
| REQ-F-014 | Dividir | Modelo físico de difusão/dispersão e discretização de fluxo; acoplamento entre componentes como extensão futura. |
| REQ-F-015 | Manter | N aberto em runtime, IDs estáveis e declaração de propriedades; sem enum fechado. |
| REQ-F-016 | Reescrever | Dependências, unidades e consistência de atualização de conjuntos acoplados. REV-12. |
| REQ-F-017 | Manter | Acoplamento substituível; implementação inicial única permitida. |
| REQ-F-018 | Dividir | Registro/criação no executável e carregamento binário externo, com marcos diferentes. REV-21. |
| REQ-F-019 | Reescrever | Capacidade declarada, restrição e evidência de combinação aprovada são estados distintos. REV-11. |
| REQ-F-020 | Manter; mover detalhe | Delegação algébrica como requisito; escolha/configuração PETSc no ADR de backend. |
| REQ-F-021 | Manter | Configuração resolvida, defaults, versões, erros em opções desconhecidas e reprodutibilidade. |
| REQ-F-022 | Dividir | Diagnósticos obrigatórios gerais e métricas específicas por capacidade/caso. |
| REQ-F-023 | Reescrever | Perfil de formato, topologia, unidades, localização, tempo e aplicabilidade definidos. REV-14. |
| REQ-F-024 | Mover parcialmente | Testabilidade no requisito de qualidade; GTest no padrão/ADR de testes, preservando cenários atuais. |
| REQ-F-025 | Reescrever | Contrato de artefatos e avaliação CRIVO sem dependência de runtime. REV-07. |
| REQ-F-026 | Manter | Erros inequívocos e testes por código/classe/contexto; não apenas captura genérica. |
| REQ-F-027 | Reescrever | Primeiro caso científico selecionado, aprovação e fronteira exata da v1.0. REV-05. |
| REQ-F-028 | Dividir | Superfície livre e wet/dry condicional com marcos/gates próprios. |
| REQ-F-029 | Manter e completar | Revisão geométrica e topológica desde o contrato, sem exigir movimento agora. |
| REQ-F-030 | Manter | Políticas abertas; não usar catálogo enum para métodos de movimento. |
| REQ-F-031 | Mover implementação para horizonte futuro | Manter admissibilidade arquitetural; retesselação/CVT só quando selecionadas. REV-15. |
| REQ-F-032 | Manter condicional | Gate de conservação e limites aplicável quando houver transferência. |
| REQ-F-033 | Reescrever | Identidade, hashes e revisões; histórico completo quando houver evolução. |
| REQ-F-034 | Manter; separar decisão e contrato | VGT continua provedor decidido; especificar adaptador, versões e equivalência. REV-02/08. |
| REQ-F-035 | Manter e completar | Ranges fortes, lifetime, adjacências, stencils e subconjuntos sem impor layout prematuro. REV-03/19. |

### 9.2 Requisitos científicos

| ID | Disposição | Ajuste / critério |
| --- | --- | --- |
| REQ-D-001 | Manter | FV como decisão científica, formalizada em ADR. |
| REQ-D-002 | Reescrever | Explicitar representação 2D, extrusão/camadas e Voronoi 3D genérico; não presumir ortogonalidade em todas as geometrias. |
| REQ-D-003 | Manter | Malha externa e responsabilidade geométrica do VMM; separar exemplos pedagógicos da cadeia de produção. |
| REQ-D-004 | Manter e completar | IDs externos versus índices densos e mapeamento recuperável. REV-09. |
| REQ-D-005 | Manter | Sem células fictícias físicas; distinguir halos de paralelismo. |
| REQ-D-006 | Manter | Invariante científico de conservação; ligar a F-008, sem duplicar texto operacional. |
| REQ-D-007 | Manter | Advecção por face com orientação explícita. |
| REQ-D-008 | Manter | Princípio de abertura temporal; vincular F-011. |
| REQ-D-009 | Manter | Exemplos de métodos não viram enumeração exaustiva nem backlog obrigatório. |
| REQ-D-010 | Reescrever | Positividade/boundedness/monotonicidade condicionadas à combinação e hipóteses verificadas. |
| REQ-D-011 | Manter | Componentes no núcleo conceitual, não implementação de todas as espécies agora. |
| REQ-D-012 | Manter | Acoplamento aberto; PCFIELDSPLIT não decide sozinho acoplamento físico. |
| REQ-D-013 | Manter | EOS guiada pelo caso; gases reais fora do escopo atual. |
| REQ-D-014 | Manter e completar | SI e conversões rastreáveis; unidades dos campos/fontes. |
| REQ-D-015 | Manter | Evolução via decisão explícita, sem reabrir fundamentos a cada método. |
| REQ-D-016 | Manter com marco | Superfície livre fundamental, mas não exigida para aprovar exemplo estacionário de difusão. |
| REQ-D-017 | Reescrever | Critérios wet/dry, limiares, transições e conservação no caso correspondente. |
| REQ-D-018 | Manter no contrato | Não assumir imobilidade irreversível; não inserir velocidades em um exemplo que só trata difusão estacionária. |
| REQ-D-019 | Manter condicional | GCL discretamente especificada com tempo/geometria no momento da implementação móvel. |
| REQ-D-020 | Manter | CVT e moving Voronoi explicitamente futuros. |
| REQ-D-021 | Reescrever | Limites de correção geométrica e métricas de alteração física, quando ativadas. |
| REQ-D-022 | Manter e completar | Invalidação de caches e mapeamentos quando muda conectividade. |
| REQ-D-023 | Mover parte para ADR | Provedor VGT e não duplicação são decisão arquitetural; preservar requisito de qualidade dos gradientes. |
| REQ-D-024 | Reescrever | Vida útil, malha/revisões e invalidação geométrica além da topológica. REV-03. |

**Remoções propostas:** retirar dos textos normativos duplicações que apenas repetem outro requisito; retirar exemplos fechados incompatíveis com a abertura numérica; retirar da lista de entregas obrigatórias arquivos de métodos não selecionados. Não remover IDs ou capacidades científicas sem substituição rastreada. Nenhum requisito científico inteiro recebe recomendação de exclusão incondicional nesta auditoria.

### 9.3 Qualidade, configuração e naming

Acrescentar a cada requisito a versão de introdução, horizonte, caso aplicável, aceite e evidência. “Consolidado” deve significar aceito na baseline, não implementado ou cientificamente aprovado.

QS-PERF-001 precisa de workload, ambiente, precisão-alvo e regra de regressão; QS-GEO-001 precisa definir o custo aceitável da abstração; QS-GEO-002/003 precisam de caso e tolerância específicos. Não é responsável fixar números universais antes de medir. QS-EXT-001 deve ser condicionado à ABI/plataforma aprovada, em coerência com D-OPEN-06.

O YAML atual em arquivo `.ini` é uma decisão pedagógica existente, não evidência de um schema definitivo de configuração física. A futura escolha de schema deve manter o nome do exemplo pedido pelo usuário e esclarecer o formato real, sem renomeações oportunistas.

O padrão novo deve preservar ou superseder explicitamente: includes C++ em ordem alfabética, depois externos, depois MOHID-NG com `<...>`; comentários `//`; separadores de exemplos; tipos próprios; constantes centralizadas; identidade por classe. Mudar nomes de funções/arquivos/namespaces não deve alterar silenciosamente identificadores de diagnóstico ou método usados em dados e evidências.

O uso de `PetscReal` em C.8 deve ficar restrito ao adaptador, ou ser trocado por `Real` no exemplo de classe própria. Valores de tolerância ilustrativos não devem redefinir implicitamente a tolerância vigente. Precisão e largura de índices de PETSc precisam ser verificadas na fronteira; não se pode assumir que toda instalação corresponde ao `Real` e ao `Integer` locais.

## 10. Gates de V&V e reprodutibilidade

### 10.1 Ficha mínima de gate

Cada gate precisa de: requisito/caso/ADR ligados; equações e hipóteses; malha e insumos identificados; parâmetros e perfil de build; referência independente; métricas/normas; limiares justificados; procedimento reexecutável; artefatos esperados; falha inequívoca; responsável pela aprovação. Gates condicionais precisam de condição explícita de aplicabilidade.

| Gate existente | Complemento necessário | Momento recomendado |
| --- | --- | --- |
| GRAD-01 | Constante, linear e manufaturada não linear; contornos e geometrias; refinamento; equivalência do adaptador/provedor. | Antes de substituir os gradientes locais. |
| DIFF-01 | Operador, sinal, fontes, contornos, norma do resíduo exato e erro de solução; convergência em malha. | Primeira linha física de verificação proposta. |
| ADV-01 | Orientação, conservação, limitações, CFL e comportamento do método escolhido. | Quando advecção for implementada. |
| N0-REPOUSO | Equilíbrio, condições de contorno e tolerância de velocidade/superfície. | Antes de casos hidrodinâmicos. |
| N1-ONDA | Amplitude/fase e refinamento espaço-temporal. | Após o esquema hidrodinâmico inicial. |
| N1-THACKER | Referência, shoreline, tratamento wet/dry e métricas ao longo do tempo. | Quando wet/dry entrar no escopo. |
| MC-01 | Registro de N componentes, unidades, independência e conservação pertinente. | Ao introduzir estado multicomponente. |
| MC-02 | Acoplamento/processos, referência independente e erro de splitting quando aplicável. | Ao ativar processos acoplados. |
| PLUGIN-01 | Descoberta, versões compatíveis/incompatíveis, colisões, falhas de carregamento e ownership. | Conforme marco da ABI, não como pré-requisito do primeiro cálculo estacionário. |
| FREE-01 | Balanço de volume e acoplamento com superfície livre. | No marco de superfície livre. |
| GCL-01 | Campo uniforme sob movimento e consistência discreta volume/fluxo geométrico no integrador escolhido. | Antes de habilitar movimento em casos científicos. |
| REMESH-01 | Transferência de constantes, conservadas e campos suaves; balanços e limites antes/depois. | Ao ativar retesselação/remapeamento. |
| CVT-01 | Métrica geométrica e efeito sobre solução/conservação; parâmetros e trajetória identificados. | Ao selecionar regularização. |

Não reutilizar a rotina de produção para fabricar o valor esperado do mesmo teste sem uma referência independente. Não usar só campos constantes para atribuir ordem a Green-Gauss. A suíte local atual verifica a constante nesse método, mas não demonstra uma campanha de convergência de todos os métodos.

### 10.2 Saída científica e manifesto

Um perfil netCDF-CF/UGRID deve definir relações topológicas, localização das variáveis e metadados compatíveis com as convenções; não basta salvar arrays em um contêiner netCDF. A convenção descreve conectividade e associação dos dados à malha. [Especificação UGRID](https://ugrid-conventions.github.io/ugrid-conventions/).

Proposta para RunManifest: identificação da execução e do código; alterações locais quando houver; hashes de malha/configuração/dados; configuração resolvida; versões e opções de dependências; precisão e índices; perfis de FP/CPU; threads/processos; método/provedor; tolerâncias e critérios de parada; unidades e convenções; seeds; revisões de malha; métricas e caminhos/hashes dos artefatos. Em movimento/restart, acrescentar as informações necessárias para reconstituir estado e histórico.

Definir separadamente repetibilidade no mesmo ambiente, equivalência numérica entre perfis e reprodutibilidade científica. Nenhum perfil com O3/native/LTO deve ser apresentado como garantia de igualdade bit a bit entre compiladores ou arquiteturas.

## 11. ADRs e decisões humanas

### 11.1 ADRs a formalizar

Os temas abaixo não recebem números definitivos neste relatório; usar a convenção e alocação oficial da planilha.

| Tema | Estado recomendado | Conteúdo mínimo e evidência de aceite |
| --- | --- | --- |
| FV como fundamento | Decisão madura | Balanços, entidades e razões; relação com requisitos científicos. |
| Voronoi e geração externa | Decisão madura | Fronteiras VMM/MOHID-NG, posição das incógnitas e evolução do contrato 2D/camadas/3D. |
| Separação física/métodos e extensibilidade | Diretriz madura | Registro aberto, seleção fora dos loops, critérios para traits/concepts e limites da primeira implementação. |
| Integração temporal substituível | Princípio maduro; algoritmo aberto | Histórico/estágios e extensibilidade, sem escolher prematuramente D-OPEN-01. |
| Advecção por face e reconstrução | Princípio maduro; esquema aberto | Orientação e composição; primeiro esquema depende do caso. |
| Componentes/processos/acoplamento | Princípio maduro | Estado extensivo/intensivo, unidades, dependências e contrato de atualização. |
| PETSc como backend prioritário | Decisão madura | Fronteira de tipos/erros, precisão/índices e primeiro solver/precondicionador a registrar. |
| VGT como provedor | Decisão da v0.14 | Adaptador, compatibilidade VMM, versões, migração dos kernels locais e gates. |
| Geometria dinâmica futura | Princípio maduro | Ownership, revisões, invalidação, responsabilidade de remapeamento; sem obrigar ALE agora. |
| Avaliação independente CRIVO | Princípio maduro | Schema de troca, evidências e aprovação externa ao runtime. |
| Licença MPL-2.0 | **Escolha feita pelo responsável** | Escopo, transição de avisos, titularidade, dependências e política de distribuição. |
| Naming e compatibilidade | A formalizar | Migração de `.h/.cc`, namespaces/macros; preservação de identidade, includes e comentários. |
| Build científico e desempenho | Proposta deste relatório | Perfis, ISA/FP, IPO/PGO, autodiscovery, runners, instalação e orçamento de regressão. |
| Configuração/saída/restart | Contratos a decidir por marco | Schema, unidades, compatibilidade, defaults, versionamento e recuperação. |

Não há arquivos ADR implementando formalmente esses temas no commit auditado. A planilha contém candidatos, o que é avanço de cadastro e não substituto do conteúdo decisório.

### 11.2 Disposição das decisões abertas existentes

| Decisão | Disposição nesta revisão |
| --- | --- |
| D-OPEN-01, integração temporal de referência | Manter aberta; desnecessária para o caso estacionário inicial. |
| D-OPEN-02, advecção/limitador | Manter aberta; não escolher pelos nomes de arquivos no Apêndice D. |
| D-OPEN-03, difusão/dispersão inicial | Fechar primeiro o caso e a discretização de referência; separar modelo físico de método de fluxo. |
| D-OPEN-04, acoplamento | Manter aberta; uma estratégia futura inicial não fecha a arquitetura. |
| D-OPEN-05, EOS | Manter aberta até haver caso que a exija. |
| D-OPEN-06, Plugin API/ABI | Manter aberta; especificar o que é prometido antes da implementação binária. |
| D-OPEN-07, configuração | Manter aberta quanto ao schema definitivo; reconhecer o YAML já usado no exemplo. |
| D-OPEN-08, primeiro caso científico 2D/v1.0 | Manter aberta. A proposta DIFF-01 não constitui aprovação automática de v1.0. |
| D-OPEN-09, ordem dos casos | Manter aberta até consultar I2 e confirmar dependências/insumos. |
| D-OPEN-10, checkpoint/restart | Manter aberta; separar capacidade de requisito temporal básico. |
| D-OPEN-11, licença | **Resolvida: MPL-2.0; planilha e LICENSE já atualizados no encerramento.** Restam consolidação do documento normativo e conformidade de distribuição. |
| D-OPEN-12, governança pós-1.0 | Manter aberta e independente da licença. |
| D-OPEN-13, superfície livre/wet-dry | Manter formulação aberta; capacidade científica preservada. |
| D-OPEN-14, primeiro movimento de malha | Manter aberta, fora da primeira implementação fixa. |
| D-OPEN-15, métricas/CVT | Manter aberta; escolher com o primeiro caso de adaptação. |
| D-OPEN-16, remapeamento conservativo | Manter aberta quanto ao método; fixar antes a responsabilidade e os invariantes. |

Questões para a próxima rodada humana, sem necessidade de decisão nesta revisão:

1. O primeiro marco físico continuará sendo DIFF-01 estacionário, distinto do caso científico de v1.0? Quem aprovará a ficha e a discretização?
2. Para integrar VGT, será priorizado um contrato de entrada independente no provedor ou um adaptador que materializa o diagrama VMM? Que informação o pacote de malha passará a transportar?
3. Quais direitos de contribuição e quais versões/licenças de VMM/VGT comporão a distribuição MPL-2.0 do projeto?
4. Quais plataformas e CPUs constituem os perfis oficialmente suportados na primeira release? Há necessidade de distribuir binários portáteis ou apenas compilar localmente?
5. Quais casos e tolerâncias definem o orçamento científico de uma otimização? Qual política usar quando a referência do erro relativo for identicamente zero?
6. Onde estão I2 e os dados executáveis dos casos históricos, e quem fornece a aprovação CRIVO com contrato versionado?

## 12. Rastreabilidade, prontidão e próximos passos

### 12.1 Situação da planilha

A planilha inicial registra 14 ações P0, 59 requisitos, 16 decisões no cadastro D-OPEN, dez candidatos a ADR, 13 gates e uma baseline de release. A aba de evidências examinada não contém registros de evidência materializada. Na leitura de encerramento, D-OPEN-11 já está definida como MPL-2.0; não deve permanecer contada como escolha em aberto.

As convenções registradas em `00_README` incluem CHG, AUD, ADR, CASE, TEST e EVID, preservando os IDs REQ existentes e os GateIDs. Portanto, o Item 11 não deve continuar a sugerir que toda a política de identificação está indefinida.

Pendências concretas a corrigir em uma próxima edição autorizada:

- Reconciliar `03_Change_Register!E2` e a nota de preenchimento Git com os hashes e PR já presentes.
- Resolver o vínculo de AUD-0001 para CHG-0002, ainda não cadastrado na aba de mudanças.
- Distinguir requisitos parcialmente atendidos pelo bootstrap dos ainda não iniciados; não marcar todos como implementados por haver conceitos similares.
- Vincular caminhos reais de testes e evidências às linhas; não promover texto descritivo de uma verificação a evidência arquivada.
- Vincular os commits MPL-2.0 já existentes e este relatório pelo procedimento de rastreabilidade, sem inventar antecipadamente IDs de mudança/evidência ou recriar D-OPEN-11.

### 12.2 Auditoria das 14 ações P0

| Ação do Item 16.2 | Situação observada | Trabalho restante |
| --- | --- | --- |
| 1. Preservar baseline | Atendida por tag e commit. | Manter referência; não recriar. |
| 2. Classificar código | Este relatório fornece classificação fundamentada. | Revisão humana e vínculo ao registro de auditoria; não foi fechado automaticamente na planilha. |
| 3. Rastreabilidade mínima | Parcial, com entrega real via PR #1. | Status, referências, templates e primeiro encadeamento completo com evidência. |
| 4. ADRs maduros | Candidatos cadastrados. | Escrever e aprovar ADRs efetivos. |
| 5. Contratos conceituais | Bootstrap de malha/campo existente. | Contratos completos de estado, capacidades e fronteiras; não criar API universal por antecipação. |
| 6. Não fechar malha para o futuro | Intenção textual presente. | Revisões e invalidação explicitadas; testes do contrato. |
| 7. Fronteira movimento/remapeamento | Direção conceitual presente. | Responsabilidade geométrica versus transferência física e artefatos. |
| 8. Padrão e automação | Ferramentas e CI prévios existem. | Compatibilizar com Apêndice C e aprovar migração; não refazer tudo mecanicamente. |
| 9. Primeira linha 2D/gates | Ainda não operacional. | Ficha DIFF-01 e separação do caso científico de v1.0. |
| 10. Decisões abertas | Registro já criado. | Donos, dependências, atualização da licença e condições de fechamento. |
| 11. Congelar árvore inicial | Árvore-alvo documental. | Corrigir existente/planejado e adiar arquivos de métodos não escolhidos. |
| 12. MeshRanges/MeshGeneration | Spans e IDs são uma base parcial. | Ownership, adjacências, revisões geométricas/topológicas e invalidadores. |
| 13. Infraestrutura mínima | FieldSet e registry de gradientes são bases parciais. | Materializar somente os contratos necessários ao marco aprovado; não há PhysicalState/RunManifest completo. |
| 14. CMake modular | Target-based e `cmake/` já existem. | Refatoração incremental com perfis, testes de descoberta e consumo; proposta na seção 8. |

**Conclusão do gate:** a v0.14 permite prosseguir com decisões e contratos focados, mas não há evidência para declarar todo o Item 16 encerrado nem para anunciar o primeiro pipeline científico concluído.

### 12.3 Sequência recomendada

1. Aprovar este relatório e registrar sua ligação à auditoria existente; reconciliar os status documentais com a decisão MPL-2.0 já registrada, sem misturar essa etapa com mudanças numéricas.
2. Reconciliar a definição v0.14 com o estado dos Blocos 0-2 e dividir o gate em prontidão conceitual e prontidão dos primeiros kernels.
3. Formalizar os ADRs que destravam trabalho: fronteiras de dados/VGT, revisões de malha, licença/distribuição, primeira linha de verificação e build científico.
4. Fechar a ficha DIFF-01: malha externa, posição da incógnita, discretização, contornos, fonte, normas e erro relativo; manter um solver e um precondicionador.
5. Definir o contrato geométrico necessário ao VGT e ao output científico, incluindo compatibilidade com os pacotes existentes; demonstrar o adaptador com testes pequenos.
6. Após autorização de implementação, reorganizar o CMake preservando comportamento e autodiscovery; verificar Debug/Release, runners e inclusão automática de novos programas antes de ativar otimizações adicionais.
7. Criar referências de desempenho e comparar IPO/native isoladamente, com gates científicos. Priorizar gargalos observados de adjacência/setup/memória e, quando existir, montagem/solução PETSc.
8. Integrar gradientes externos apenas depois de contrato, conformidade e equivalência verificadas; preservar testes históricos e registrar a substituição.
9. Implementar o pipeline aprovado e materializar a primeira cadeia completa requisito → ADR → mudança → teste → evidência → avaliação → release.
10. Expandir advecção, transientes, multicomponentes, superfície livre e 3D conforme casos e gates. PGO, métodos adicionais e paralelismo entram com cargas e critérios próprios, não como promessa de desempenho antecipada.

## 13. Encerramento

A melhor contribuição do CMake do VMM é a organização de responsabilidades e a capacidade de configurar o projeto por targets. Para o MOHID-NG, essa base deve ser adaptada para impedir perfis inconsistentes, instrumentação acidental, dependências implícitas e perda de portabilidade.

**Recomendação final:** adotar o padrão modular com os ajustes propostos; preservar descoberta automática e runners; iniciar o estudo de desempenho por Release/IPO/native controlados e perfilamento do custo real. Manter fast-math global desligado. Registrar MPL-2.0 como decisão tomada e tratar separadamente sua aplicação e as obrigações das dependências.

Não há alteração de implementação entregue nesta tarefa. Este arquivo é a entrada documental para a próxima rodada de decisão e autorização de trabalho.
