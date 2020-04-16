#ifndef H_BMP
#define H_BMP
#include <stdio.h>
#include "conf.h"
#pragma warning(disable : 4996)
/*---------------------------------------------------------------------------------------------*/

#pragma pack(1)

struct cabecalho {
	unsigned short tipo;
	unsigned int tamanho_arquivo;
	unsigned short reservado1;
	unsigned short reservado2;
	unsigned int offset;
	unsigned int tamanho_image_header;
	int largura;
	int altura;
	unsigned short planos;
	unsigned short bits_por_pixel;
	unsigned int compressao;
	unsigned int tamanho_imagem;
	int largura_resolucao;
	int altura_resolucao;
	unsigned int numero_cores;
	unsigned int cores_importantes;
};
typedef struct cabecalho CABECALHO;

struct rgb {
	unsigned char blue;
	unsigned char green;
	unsigned char red;
};
typedef struct rgb RGB;

struct xy {
	int x;
	int y;
};
typedef struct xy XY;

 //#pragma pack(pop)

/*---------------------------------------------------------------------------------------------*/

void informacao(CABECALHO cabecalho);
RGB** cria_matriz_imagem(int altura, int largura);
XY* cria_matriz_sola_XY(int tamanho);
void libera_matriz_imagem(RGB **mat, int altura);
void libera_matriz_sola_XY(XY *vet);
void zera_histograma(int vetor[256]);
void le_imagem_e_histograma(int altura, int largura, RGB **matriz, int vetor[256], FILE *bmp, char saida[], CABECALHO cabecalho);
int metodo_otsu(int vetor[256], int altura, int largura);
int aplica_metodo_otsu(int altura, int largura, RGB **matriz, int limiar, XY *pontos, XY *estremidade, int *estremidadeQuantidade, char saida[], CABECALHO cabecalho);
int dilatacao(int altura, int largura, RGB **matriz, XY *pontos, int pixeisQuantidade);
void imagem_branca(RGB **matriz, XY *pontos, int pixeisQuantidade);
void distancia_pontos(int *x1, int *y1, int *x2, int *y2, XY *estremidade, int estremidadeQuantidade);
void calcular_angulo(int *xmeio, int *ymeio, double *anguloCos, double *anguloSin, int *xtemp, int *ytemp, int altura, int largura, int x1, int y1, int x2, int y2,FILE *angulo,char saida[TAMANHO_MAXIMO_STRING]);
void converte_tons_cinza(char entrada[TAMANHO_MAXIMO_STRING], char saida[TAMANHO_MAXIMO_STRING],FILE *angulo);

#endif // H_BMP
