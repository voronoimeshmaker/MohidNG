# Tratamento de dependências

O Mohid-NG utiliza e prevê utilizar diversas bibliotecas externas,
ferramentas de desenvolvimento e backends científicos/HPC opcionais.

O inventário de referência do software de terceiros efetivamente utilizado,
distribuído ou requerido pelo projeto é mantido em
`THIRD_PARTY_NOTICES.md` e deverá evoluir com a implementação.

As dependências deverão ser classificadas, quando aplicável, como:

- dependências diretas de execução ou de compilação;
- backends científicos ou HPC opcionais;
- pacotes de solvers, particionamento ou cálculo numérico acedidos através
  de outra dependência;
- dependências de testes e desenvolvimento;
- dependências de documentação; e
- dependências transitivas relevantes para redistribuição ou conformidade
  com licenças.

Cada componente de terceiros permanece sujeito à sua própria licença.

Não se deve assumir que a licença de uma dependência principal abrange os
pacotes que essa dependência descarrega, compila, liga ou disponibiliza por
outros meios. As dependências transitivas e opcionais relevantes deverão,
por isso, ser analisadas separadamente quando passarem a fazer parte de uma
configuração suportada do Mohid-NG.

As novas dependências deverão ser adicionadas a `THIRD_PARTY_NOTICES.md`
quando forem adotadas, incluindo a respetiva função, licença, modo de
integração e quaisquer considerações de redistribuição ou compatibilidade
relevantes para o Mohid-NG.