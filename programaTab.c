#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição dos tipos de dados
typedef struct RegistroCabecalho {
  char status;
  int topo;
  long long proxByteOffset;
  int nroRegArq;
  int nroRegRem;
} RegistroCabecalho;

typedef struct RegistroDados {
  char removido;
  int tamanhoRegistro;
  long long prox;
  int id;
  int idade;
  int tamNomeJog;
  char nomeJogador[1];
  int tamNacionalidade;
  char nacionalidade[1];
  int tamNomeClube;
  char nomeClube[1];
} RegistroDados;

void inserirRegistro(FILE *arquivo, RegistroDados registro) {
  // Abrindo o arquivo no modo binário para atualização
  // Posicionando o cursor no final do arquivo
  fseek(arquivo, 0, SEEK_END);

  // Gravando o registro no arquivo
  fwrite(&registro, sizeof(RegistroDados), 1, arquivo);

  // Atualizando o registro de cabeçalho
  RegistroCabecalho cabecalho;
  fseek(arquivo, 0, SEEK_SET);
  fread(&cabecalho, sizeof(RegistroCabecalho), 1, arquivo);

  cabecalho.nroRegArq++;
  fseek(arquivo, 0, SEEK_SET);
  fwrite(&cabecalho, sizeof(RegistroCabecalho), 1, arquivo);

}


void criarArquivoBinarioCSV(char *nomeArquivoBin, char *nomeArquivoCSV) {
  // Abrindo o arquivo CSV no modo de leitura
  FILE *arquivoCSV = fopen(nomeArquivoCSV, "r");

  // Verificando se o arquivo CSV foi aberto com sucesso
  if (arquivoCSV == NULL) {
    printf("Erro ao abrir o arquivo CSV %s.\n", nomeArquivoCSV);
    exit(1);
  }

  // Abrindo o arquivo binário no modo binário para escrita
  FILE *arquivoBin = fopen(nomeArquivoBin, "wb");

  // Verificando se o arquivo binário foi aberto com sucesso
  if (arquivoBin == NULL) {
    printf("Erro ao abrir o arquivo binário %s.\n", nomeArquivoBin);
    exit(1);
  }

  // Criando o registro de cabeçalho
  RegistroCabecalho cabecalho;
  cabecalho.status = '0';
  cabecalho.topo = -1;
  cabecalho.proxByteOffset = 0;
  cabecalho.nroRegArq = 0;
  cabecalho.nroRegRem = 0;

  // Escrevendo o registro de cabeçalho no arquivo binário
  fwrite(&cabecalho, sizeof(RegistroCabecalho), 1, arquivoBin);

  // Lendo cada linha do arquivo CSV
  char linha[1024];
  while (fgets(linha, sizeof(linha), arquivoCSV)) {
    // Dividindo a linha em campos
    char *campos[5];
    int i = 0;
    char *token = strtok(linha, ",");
    while (token != NULL) {
      campos[i++] = token;
      token = strtok(NULL, ",");
    }

    // Criando um novo registro
    RegistroDados registro;
    registro.removido = '0';
    registro.tamanhoRegistro = sizeof(RegistroDados);
    registro.prox = -1;
    registro.id = atoi(campos[0]);
    registro.idade = atoi(campos[1]);
    strcpy(registro.nomeJogador, campos[2]);
    strcpy(registro.nacionalidade, campos[3]);
    strcpy(registro.nomeClube, campos[4]);

    // Inserindo o registro no arquivo binário
    inserirRegistro(arquivoBin, registro);
  }

  // Fechando os arquivos
  fclose(arquivoCSV);
  fclose(arquivoBin);
}

void lerTodosOsRegistros(char *nomeArquivo) {
  // Abrindo o arquivo no modo binário para leitura
  FILE *arquivo = fopen(nomeArquivo, "rb");

  // Verificando se o arquivo foi aberto com sucesso
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
    exit(1);
  }

  // Posicionando o cursor no início do arquivo
  fseek(arquivo, 0, SEEK_SET);

  // Lendo o registro de cabeçalho
  RegistroCabecalho cabecalho;
  fread(&cabecalho, sizeof(RegistroCabecalho), 1, arquivo);

  // Percorrendo o arquivo e lendo todos os registros
  int nroRegLidos = 0;
  while (nroRegLidos < /*cabecalho.nroRegArq*/ 1000) {
    RegistroDados registro;
    fread(&registro, sizeof(RegistroDados), 1, arquivo);

    // Se o registro não estiver removido, exiba seus dados
    if (registro.removido == '0') {
      printf("\nID: %d\n", registro.id);
      printf("Idade: %d\n", registro.idade);
      printf("Nome do Jogador: %s\n", registro.nomeJogador);
      printf("Nacionalidade: %s\n", registro.nacionalidade);
      printf("Nome do Clube: %s\n", registro.nomeClube);
    }

    nroRegLidos++;
  }

  // Fechando o arquivo
  fclose(arquivo);
}

void lerRegistro(char *nomeArquivo, int id) {
  // Abrindo o arquivo no modo binário para leitura
  FILE *arquivo = fopen(nomeArquivo, "rb");

  // Verificando se o arquivo foi aberto com sucesso
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
    exit(1);
  }

  // Posicionando o cursor no início do arquivo
  fseek(arquivo, 0, SEEK_SET);

  // Lendo o registro de cabeçalho
  RegistroCabecalho cabecalho;
  fread(&cabecalho, sizeof(RegistroCabecalho), 1, arquivo);

  // Percorrendo o arquivo buscando o registro com o ID especificado
  int encontrado = 0;
  RegistroDados registro;
  while (!feof(arquivo) && !encontrado) {
    fread(&registro, sizeof(RegistroDados), 1, arquivo);

    if (registro.id == id) {
      encontrado = 1;

      // Exibindo os dados do registro
      printf("ID: %d\n", registro.id);
      printf("Idade: %d\n", registro.idade);
      printf("Nome do Jogador: %s\n", registro.nomeJogador);
      printf("Nacionalidade: %s\n", registro.nacionalidade);
      printf("Nome do Clube: %s\n", registro.nomeClube);
    }
  }

  // Mensagem caso o registro não seja encontrado
  if (!encontrado) {
    printf("Registro inexistente.\n");
  }

  // Fechando o arquivo
  fclose(arquivo);
}

void buscarRegistros(char *nomeArquivo, int numBuscas, ...) {
  // Abrindo o arquivo no modo binário para leitura
  FILE *arquivo = fopen(nomeArquivo, "rb");

  // Verificando se o arquivo foi aberto com sucesso
  if (arquivo == NULL) {
    printf("Erro ao abrir o arquivo %s.\n", nomeArquivo);
    exit(1);
  }

  // Posicionando o cursor no início do arquivo
  fseek(arquivo, 0, SEEK_SET);

  // Lendo o registro de cabeçalho
  RegistroCabecalho cabecalho;
  fread(&cabecalho, sizeof(RegistroCabecalho), 1, arquivo);

  // Realizando as buscas
  va_list argumentos;
  va_start(argumentos, numBuscas);
  for (int i = 0; i < numBuscas; i++) {
    int numCampos = va_arg(argumentos, int); // Número de campos na busca

    // Montando a string de busca
    char busca[255];
    strcpy(busca, "");
    for (int j = 0; j < numCampos; j++) {
      char campo[50];
      strcpy(campo, va_arg(argumentos, char*)); // Nome do campo
      char valor[50];
      strcpy(valor, va_arg(argumentos, char*)); // Valor do campo

      strcat(busca, campo);
      strcat(busca, "=");
      strcat(busca, valor);
      if (j < numCampos - 1) {
        strcat(busca, " AND ");
      }
    }

    // Percorrendo o arquivo buscando registros que atendem ao critério
    int encontrado = 0;
    RegistroDados registro;
    while (!feof(arquivo) && !encontrado) {
      fread(&registro, sizeof(RegistroDados), 1, arquivo);

      // Verificando se o registro satisfaz a busca (implementar a lógica de comparação)
      int satisfazBusca = 1; // Substituir por lógica de comparação com 'busca'

      if (satisfazBusca) {
        encontrado = 1;

        // Exibindo os dados do registro
        printf("Busca %d:\n", i + 1);
        printf("ID: %d\n", registro.id);
        printf("Idade: %d\n", registro.idade);
        printf("Nome do Jogador: %s\n", registro.nomeJogador);
        printf("Nacionalidade: %s\n", registro.nacionalidade);
        printf("Nome do Clube: %s\n", registro.nomeClube);
      }
    }

    // Mensagem caso o registro não seja encontrado na busca específica
    if (!encontrado) {
      printf("Registro inexistente na busca %d.\n", i + 1);
    }
  }
  va_end(argumentos);

  // Fechando o arquivo
  fclose(arquivo);
}

int main(int argc, char *argv[]) {
  // Verificando o número de argumentos
  if (argc < 3) {
    printf("Uso: programaTrab <funcionalidade> <arquivo> [...]\n");
    exit(1);
  }

  // Funcionalidade 1: Criar arquivo binário
  if (strcmp(argv[1], "1") == 0) {
    if (argc != 4) {
      printf("Erro: número de argumentos inválido para a funcionalidade 1.\n");
      exit(1);
    }
    criarArquivoBinarioCSV(argv[2], argv[3]);
  }

  // Funcionalidade 2: Ler todos os registros
  else if (strcmp(argv[1], "2") == 0) {
    if (argc != 3) {
      printf("Erro: número de argumentos inválido para a funcionalidade 2.\n");
      exit(1);
    }
    lerTodosOsRegistros(argv[2]);
  }

  // Funcionalidade 3: Buscar registros por critério
  else if (strcmp(argv[1], "3") == 0) {
    if (argc < 5) {
      printf("Erro: número de argumentos inválido para a funcionalidade 3.\n");
      exit(1);
    }
    int numBuscas = atoi(argv[3]); // Número de buscas informadas
    buscarRegistros(argv[2], numBuscas, argv + 4); // Passando argumentos a partir do quarto
  }

  // Funcionalidade inválida
  else {
    printf("Funcionalidade inválida. Digite 1, 2 ou 3.\n");
  }

  return 0;
}

int compararRegistro(RegistroDados registro, char *busca) {
  // Dividindo a string de busca em termos
  char termos[50][50];
  int numTermos = 0;
  char *token = strtok(busca, " ");
  while (token != NULL) {
    strcpy(termos[numTermos], token);
    numTermos++;
    token = strtok(NULL, " ");
  }

  // Percorrendo os termos da busca e verificando se cada um é satisfeito pelo registro
  for (int i = 0; i < numTermos; i++) {
    char campo[50], valor[50];
    sscanf(termos[i], "%[^=]=%s", campo, valor);

    // Comparando o campo e valor com os dados do registro
    if (strcmp(campo, "id") == 0) {
      if (atoi(valor) != registro.id) {
        return 0;
      }
    } else if (strcmp(campo, "idade") == 0) {
      if (atoi(valor) != registro.idade) {
        return 0;
      }
    } else if (strcmp(campo, "nomeJogador") == 0) {
      if (strstr(registro.nomeJogador, valor) == NULL) {
        return 0;
      }
    } else if (strcmp(campo, "nacionalidade") == 0) {
      if (strstr(registro.nacionalidade, valor) == NULL) {
        return 0;
      }
    } else if (strcmp(campo, "nomeClube") == 0) {
      if (strstr(registro.nomeClube, valor) == NULL) {
        return 0;
      }
    }
  }

  // Se todos os termos da busca forem satisfeitos, retorna 1
  return 1;
}
