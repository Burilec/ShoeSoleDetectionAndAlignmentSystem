#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>

#include "conf.h"
#include "bmb.h"
#pragma warning(disable : 4996)

/*---------------------------------------------------------------------------------------------*/

//FILE *leNomeArquivo(/*char *nomeArq*/) {
//
//
//	if (arq == NULL) {
//		printf("Erro na abertura do arquivo de entrada\n");
//		exit(0);
//	}
//
//	return(arq);
//}

/*---------------------------------------------------------------------------------------------*/

void nomeArquivo(char nomeArquivoEntrada[], char nomeArquivoSaida[])
{
	strcpy(nomeArquivoSaida, "saida/");
	int i = strlen(nomeArquivoEntrada);
	char aux[TAMANHO_MAXIMO_STRING];
	strcpy(aux, nomeArquivoEntrada);
	if (nomeArquivoEntrada[i - 1] == '\n')
	{
		nomeArquivoEntrada[i - 1] = '\0';
	}

	char *p;
	p = strtok(aux, "/\n");
	p = strtok(NULL, "/\n\0");
	strcat(nomeArquivoSaida, p);
	//strcpy(nomeArquivoSaida + n, nomeArquivoEntrada + i+1);
}

/*---------------------------------------------------------------------------------------------*/

int main(/*int argc, char **argv*/) {
	char nomeArquivoEntrada[TAMANHO_MAXIMO_STRING], nomeArquivoSaida[TAMANHO_MAXIMO_STRING];

	/*if (argc != 3) {
		printf("Numeros de parametros invalido\n");
	} else {*/
	FILE *arqin = NULL;
	arqin = fopen("arquivos.txt", "rt");

	if (arqin == NULL)
	{
		printf("Nao foi possivel abrir o arquivo do nome das imagens.");
	}

	FILE *arqout = NULL;
	arqout = fopen("Angulo.txt", "wt");

	if (arqout == NULL)
	{
		printf("Nao foi possivel abrir o arquivo do angulo de saida.");
	}

	//arq = leNomeArquivo(/*argv[1]*/);

	while (fscanf(arqin, "%s", nomeArquivoEntrada) == 1) {
		nomeArquivo(nomeArquivoEntrada, nomeArquivoSaida);
		//le_cabecalho_arquivo(nomeArquivoEntrada);
		converte_tons_cinza(nomeArquivoEntrada, nomeArquivoSaida, arqout);
	}
	fclose(arqin);
	fclose(arqout);
	/*}*/
}

/*---------------------------------------------------------------------------------------------*/