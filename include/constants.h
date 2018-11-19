#ifndef __constants__
#define	__constants__

#define TRUE 1
#define FALSE 0
#define MAX_FILES 10
#define ID_SUPERBLOCO "T2FS"
#define VERSAO_SUPERBLOCO 0x7E2
#define TAM_SUPERBLOCO 1
#define TAM_SETOR 256 //bytes
#define TAM_NOME_ARQUIVO 256

#define ERRO_LEITURA_SUPERBLOCO "Erro inesperado na leitura de informacoes do superbloco, o programa será terminado."
/*
* O bloco é formado por uma sequência de setores.
* Cluster = conjuntos contíguos de n setores lógicos.
* Clusters 0 e 1 == RESERVADOS.
* PATHNAME:
*   - começando por / : absoluto
*   - cc. : relativo
*
* A  FAT  é  organizada  como  uma  lista  com  C  elementos  de  32  bits  (4  bytes).  Cada  elemento  dessa  lista  está
* associado a um cluster de dados. Portanto, a quantidade de elementos válidos nessa lista é igual à quantidade
* total  de  clusters  da  área  de  dados.  Cada  elemento  da  FAT  pode  receber  um  valor  conforme  a  tabela  2.  As
* entradas 0 e 1 da FAT são reservadas e, por consequência, os clusters 0 e 1 existem no disco, mas não podem ser
* usados (reservados pelo T2FS).
*
* TAMANHO DO DIRETORIO = 1 CLUSTER; Isso  implica  que  um  diretório  terá  uma  quantidade  máxima  de
entradas  que  dependerá  da  quantidade  de  setores  que  formam  o  cluster. Um erro  deverá  ser gerado  caso  se
tente criar mais arquivos que o número de entradas disponíveis em um diretório.
*
*
*
*
*
*
*/

// FAT
#define BAD_SECTOR 0xfffffffe
//#define EOF 0xffffffff

#endif
