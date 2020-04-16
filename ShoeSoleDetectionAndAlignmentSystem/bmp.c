#include "bmb.h"
#include "conf.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <math.h>
#include <ctype.h>
#pragma warning(disable : 4996)
/*---------------------------------------------------------------------------------------------*/

void altera_nome_saida(char padrao[TAMANHO_MAXIMO_STRING], char alterado[TAMANHO_MAXIMO_STRING])
{
	char aux[TAMANHO_MAXIMO_STRING], meio[TAMANHO_MAXIMO_STRING];
	char *p;

	strcpy(meio, alterado);
	strcpy(aux, padrao);
	p = strtok(aux, ".\0\n");
	strcpy(alterado, p);
	strcat(alterado, meio);
	p = strtok(NULL, ".\0\n");
	strcat(alterado, p);
}

/*---------------------------------------------------------------------------------------------*/

void escreve_na_imagem(RGB ** matriz, int altura, int largura, char saida[TAMANHO_MAXIMO_STRING], CABECALHO cabecalho)
{
	FILE *saidaBMP = fopen(saida, "wb");

	if (saidaBMP == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", saida);
		exit(0);
	}

	fwrite(&cabecalho, sizeof(CABECALHO), 1, saidaBMP);

	int i, j;

	for (i = 0; i < altura; i++)
	{
		for (j = 0; j < largura; j++)
		{
			fwrite(&matriz[i][j], sizeof(RGB), 1, saidaBMP);//Escreve para o novo arquivo.
		}
	}
	fclose(saidaBMP);
}

/*---------------------------------------------------------------------------------------------*/

RGB** cria_matriz_imagem(int altura, int largura)
{
	RGB **mat;
	int i;

	mat = (RGB**)malloc(sizeof(RGB*) * altura);

	for (i = 0; i < altura; i++)
	{
		mat[i] = (RGB*)malloc(sizeof(RGB) * largura);
	}

	return mat;
}

/*---------------------------------------------------------------------------------------------*/

XY* cria_matriz_sola_XY(int tamanho)
{
	XY *vet;

	vet = (XY*)malloc(sizeof(XY) * tamanho);

	return vet;
}

/*---------------------------------------------------------------------------------------------*/

void libera_matriz_imagem(RGB **mat, int altura)
{
	int i;

	for (i = 0; i < altura; i++)
	{
		free(mat[i]);
	}

	free(mat);
}

/*---------------------------------------------------------------------------------------------*/

void libera_matriz_sola_XY(XY *vet)
{
	free(vet);
}

/*---------------------------------------------------------------------------------------------*/

void zera_histograma(int vetor[256])
{
	int i;

	for (i = 0; i < 256; i++)
	{
		vetor[i] = 0;
	}
}

/*---------------------------------------------------------------------------------------------*/

void le_imagem_e_histograma(int altura, int largura, RGB **matriz, int vetor[256], FILE *bmp, char saida[], CABECALHO cabecalho)
{
	char  nomeStringSaida[TAMANHO_MAXIMO_STRING] = { "_tons_de_cinza." };

	altera_nome_saida(saida, nomeStringSaida);

	int i, j, media;

	for (i = 0; i < altura; i++)
	{
		for (j = 0; j < largura; j++)
		{
			fread(&matriz[i][j], sizeof(RGB), 1, bmp);
			media = (matriz[i][j].red + matriz[i][j].green + matriz[i][j].blue) / 3;
			vetor[media]++;
			matriz[i][j].red = media;
			matriz[i][j].green = media;
			matriz[i][j].blue = media;
		}
	}

	escreve_na_imagem(matriz, altura, largura, nomeStringSaida, cabecalho);
}

/*---------------------------------------------------------------------------------------------*/

int metodo_otsu(int vetor[256], int altura, int largura)
{
	int i, limiar;
	double fundoPeso, fundoMeio, frentePeso, frenteMeio;
	double soma, somaB, max;

	fundoPeso = frentePeso = max = soma = somaB = limiar = 0;

	for (i = 0; i < 256; i++)
	{
		soma += i * vetor[i];
	}

	for (i = 0; i < 256; i++)
	{
		double temporarioLimiar;

		temporarioLimiar = fundoMeio = frenteMeio = 0;

		fundoPeso += vetor[i];

		if (!fundoPeso)
		{
			continue;
		}

		frentePeso = altura * largura - fundoPeso;

		if (!frentePeso)
		{
			break;
		}

		somaB += (double)(i * vetor[i]);

		fundoMeio = somaB / fundoPeso;
		frenteMeio = (soma - somaB) / frentePeso;

		temporarioLimiar = (double)fundoPeso*(double)frentePeso*(fundoMeio - frenteMeio)*(fundoMeio - frenteMeio);

		if (temporarioLimiar > max)
		{
			max = temporarioLimiar;
			limiar = i;
		}
	}
	return limiar;
}

/*---------------------------------------------------------------------------------------------*/

int aplica_metodo_otsu(int altura, int largura, RGB **matriz, int limiar, XY *pontos, XY *estremidade, int *estremidadeQuantidade, char saida[], CABECALHO cabecalho)
{
	char nomeStringSaida[TAMANHO_MAXIMO_STRING]= { "_binarizada." };
	
	altera_nome_saida(saida, nomeStringSaida);

	unsigned short media;
	int i, j;
	int pixeisQuantidade = 0;
	int temporarioEstremidadeQuantidade = *estremidadeQuantidade = 0;

	for (i = 0; i < altura; i++)
	{
		int flag = 1;

		for (j = 0; j < largura; j++)
		{
			media = (matriz[i][j].red + matriz[i][j].green + matriz[i][j].blue) / 3;

			if (limiar <= media)
			{
				matriz[i][j].red = 255;
				matriz[i][j].green = 255;
				matriz[i][j].blue = 255;
			}
			else
			{
				if (flag)
				{
					flag = 0;
					estremidade[temporarioEstremidadeQuantidade].y = i;
					estremidade[temporarioEstremidadeQuantidade].x = j;
					temporarioEstremidadeQuantidade++;
				}
				matriz[i][j].red = 0;
				matriz[i][j].green = 0;
				matriz[i][j].blue = 0;

				pontos[pixeisQuantidade].y = i;
				pontos[pixeisQuantidade].x = j;

				pixeisQuantidade++;
			}
			/*fwrite(&pixel[i][j], sizeof(RGB), 1, fout);*/
		}
		if (!flag)
		{
			estremidade[temporarioEstremidadeQuantidade].y = pontos[pixeisQuantidade - 1].y;
			estremidade[temporarioEstremidadeQuantidade].x = pontos[pixeisQuantidade - 1].x;
			temporarioEstremidadeQuantidade++;
		}
	}
	*estremidadeQuantidade = temporarioEstremidadeQuantidade;

	escreve_na_imagem(matriz, altura, largura, nomeStringSaida, cabecalho);

	return pixeisQuantidade;
}

/*---------------------------------------------------------------------------------------------*/

int dilatacao(int altura, int largura, RGB **matriz, XY *pontos, int pixeisQuantidade)
{
	int k, l, m, i, j;
	int margem = 4;
	int valor = 145;
	for (k = 0; k < 1; k++)
	{
		for (i = margem; i < altura - margem; i++)
		{
			int flag = 0;

			for (j = margem; j < largura - margem; j++)
			{
				if (matriz[i][j].red == 0)
				{
					flag = 1;

				}
				else {
					if (flag)
					{
						flag = 1;
						int cont = 0;

						for (l = i - margem; l < i + margem; l++)
						{
							for (m = j - margem; m < j + margem; m++)
							{
								if (matriz[l][m].red == 0)
								{
									cont += abs(l - i) + abs(m - j);
								}
							}
						}
						if (cont >= valor)
						{
							matriz[i][j].red = 0;
							matriz[i][j].green = 0;
							matriz[i][j].blue = 0;

							pontos[pixeisQuantidade].y = i;
							pontos[pixeisQuantidade].x = j;

							pixeisQuantidade++;
						}
						else
						{
							/*if (!cont)
							{*/
							flag = 0;
							/*}*/
						}
					}
				}
			}
		}
		for (i = altura - margem; i > margem; i--)
		{
			int flag = 0;

			for (j = largura - margem; j > margem; j--)
			{
				if (matriz[i][j].red == 0)
				{
					flag = 1;
				}
				else {
					if (flag)
					{
						flag = 1;
						int cont = 0;

						for (l = i - margem; l < i + margem; l++)
						{
							for (m = j - margem; m < j + margem; m++)
							{
								if (matriz[l][m].red == 0)
								{
									cont += abs(l - i) + abs(m - j);
								}
							}
						}
						if (cont >= valor)
						{
							matriz[i][j].red = 0;
							matriz[i][j].green = 0;
							matriz[i][j].blue = 0;

							pontos[pixeisQuantidade].y = i;
							pontos[pixeisQuantidade].x = j;

							pixeisQuantidade++;
						}
						else
						{
							/*if (!cont)
							{*/
							flag = 0;
							/*}*/
						}
					}
				}
			}
		}
	}

	return pixeisQuantidade;
}

/*---------------------------------------------------------------------------------------------*/

void imagem_branca(RGB **matriz, XY *pontos, int pixeisQuantidade)
{
	int i;

	for (i = 0; i < pixeisQuantidade; i++)
	{
		matriz[pontos[i].y][pontos[i].x].red = 255;
		matriz[pontos[i].y][pontos[i].x].green = 255;
		matriz[pontos[i].y][pontos[i].x].blue = 255;
	}
}

/*---------------------------------------------------------------------------------------------*/

void distancia_pontos(int *x1, int *y1, int *x2, int *y2, XY *estremidade, int estremidadeQuantidade)
{
	int i, j;
	double distancia = 0;

	for (i = 0; i < estremidadeQuantidade; i++)
	{
		for (j = i + 1; j < estremidadeQuantidade; j++)
		{
			double temp;
			temp = sqrt((estremidade[j].x - estremidade[i].x)*(estremidade[j].x - estremidade[i].x) + (estremidade[j].y - estremidade[i].y)*(estremidade[j].y - estremidade[i].y));

			if (temp > distancia)
			{
				distancia = temp;
				*x1 = estremidade[i].x;
				*x2 = estremidade[j].x;
				*y1 = estremidade[i].y;
				*y2 = estremidade[j].y;
			}
		}
	}
}

/*---------------------------------------------------------------------------------------------*/

void rotaciona_e_realoca(RGB ** matriz, XY *pontos, int xmeio, int ymeio, double anguloCos, double anguloSin, int xDeslocamento, int yDeslocamento, int pixeisQuantidade)
{
	int i;
	for (i = 0; i < pixeisQuantidade; i++)
	{
		int x, y;
		x = pontos[i].x -= xmeio;
		y = pontos[i].y -= ymeio;

		pontos[i].x = (int)(anguloCos * x - (anguloSin * y) + xDeslocamento);
		pontos[i].y = (int)(anguloSin * x + anguloCos * y + yDeslocamento);

		matriz[pontos[i].y][pontos[i].x].red = 0;
		matriz[pontos[i].y][pontos[i].x].green = 0;
		matriz[pontos[i].y][pontos[i].x].blue = 0;
	}
}

/*-------------------------------------------------------------------------------------------- - */

void calcular_angulo(int *xmeio, int *ymeio, double *anguloCos, double *anguloSin, int *xtemp, int *ytemp, int altura, int largura, int x1, int y1, int x2, int y2, FILE *angulo, char saida[TAMANHO_MAXIMO_STRING])
{


	char string[TAMANHO_MAXIMO_STRING];
	
	strcpy(string, saida);

	char *p = strtok(string, "/\n\0");
	p = strtok(NULL, "/\n\0");
	strcpy(string, p);

	*xmeio = (x1 + x2) / 2;
	*ymeio = (y1 + y2) / 2;

	*ymeio = *ymeio - altura / 2;
	*xmeio = *xmeio - largura / 2;

	double anguloRad, anguloGrau;
	anguloRad = atan((double)(y2 - y1) / (x2 - x1));
	anguloGrau = anguloRad * 180 / PI;
	fprintf(angulo, "Angulo da imagem %s � de %lf\n", string, anguloGrau);

	if (anguloGrau > 0)
	{
		anguloGrau = 270 - anguloGrau;
	}
	else
	{
		anguloGrau = 90 - anguloGrau;
	}


	anguloRad = anguloGrau / 180 * PI;



	fprintf(angulo, "Angulo que a imagem %s deve rotacionar é %lf\n\n", string, anguloGrau);

	*anguloCos = cos(anguloRad);
	*anguloSin = sin(anguloRad);
	*xtemp = (int)((largura / 2) - (*anguloCos * (largura / 2) - (*anguloSin * (altura / 2))));
	*ytemp = (int)((altura / 2) - (*anguloSin * (largura / 2) + *anguloCos * (altura / 2)));
}

/*-------------------------------------------------------------------------------------------- - */

int verifica_lado(RGB**matriz, int altura, int largura)
{
	int i;
	int flag1 = 1, flag2 = 1;
	int tempx1alto, tempy1alto, tempx2alto, tempy2alto, tempx1baixo, tempy1baixo, tempx2baixo, tempy2baixo;
	int alto = altura / 4, baixo = altura / 4 + altura / 2;

	for (i = 0; i < largura; i++)
	{
		if (matriz[alto][i].red == 0)
		{
			if (flag1) {
				flag1 = 0;
				tempx1alto = i;
				tempy1alto = alto;
			}
			else
			{
				tempx2alto = i;
				tempy2alto = alto;
			}
		}
		if (matriz[baixo][i].red == 0)
		{
			if (flag2) {
				flag2 = 0;
				tempx1baixo = i;
				tempy1baixo = baixo;
			}
			else
			{
				tempx2baixo = i;
				tempy2baixo = baixo;
			}
		}
	}

	double temporarioTamanhoAlto = sqrt((tempx2alto - tempx1alto)*(tempx2alto - tempx1alto) + (tempy2alto - tempy1alto)*(tempy2alto - tempy1alto));
	double temporarioTamanhoBaixo = sqrt((tempx2baixo - tempx1baixo)*(tempx2baixo - tempx1baixo) + (tempy2baixo - tempy1baixo)*(tempy2baixo - tempy1baixo));

	if (temporarioTamanhoBaixo < temporarioTamanhoAlto) {
		return 1;
	}
	else
	{
		return 0;
	}
}

/*-------------------------------------------------------------------------------------------- - */

void converte_tons_cinza(char entrada[TAMANHO_MAXIMO_STRING], char saida[TAMANHO_MAXIMO_STRING], FILE *angulo)
{
	CABECALHO cabecalho;
	RGB **pixel;
	XY *sola, *estremidade;
	int histograma[256], limiar;

	FILE *entradaBMP = fopen(entrada, "rb");

	if (entradaBMP == NULL)
	{
		printf("Erro ao abrir o arquivo %s\n", entrada);
		exit(0);
	}

	fread(&cabecalho, sizeof(CABECALHO), 1, entradaBMP);

	pixel = cria_matriz_imagem(cabecalho.altura, cabecalho.largura);

	zera_histograma(histograma);

	le_imagem_e_histograma(cabecalho.altura, cabecalho.largura, pixel, histograma, entradaBMP, saida, cabecalho);

	fclose(entradaBMP);

	limiar = metodo_otsu(histograma, cabecalho.altura, cabecalho.largura);

	sola = cria_matriz_sola_XY(cabecalho.altura * cabecalho.largura);

	estremidade = cria_matriz_sola_XY(cabecalho.altura * 2);

	int estremidadeQuantidade;

	int pixeisQuantidade = aplica_metodo_otsu(cabecalho.altura, cabecalho.largura, pixel, limiar, sola, estremidade, &estremidadeQuantidade, saida, cabecalho);

	imagem_branca(pixel, sola, pixeisQuantidade);

	int x1, x2, y1, y2;

	distancia_pontos(&x1, &y1, &x2, &y2, estremidade, estremidadeQuantidade);

	int xmeio, ymeio;
	double anguloCos;
	double anguloSin;
	int xtemp;
	int ytemp;

	calcular_angulo(&xmeio, &ymeio, &anguloCos, &anguloSin, &xtemp, &ytemp, cabecalho.altura, cabecalho.largura, x1, y1, x2, y2, angulo, saida);

	rotaciona_e_realoca(pixel, sola, xmeio, ymeio, anguloCos, anguloSin, xtemp, ytemp, pixeisQuantidade);

	if (verifica_lado(pixel, cabecalho.altura, cabecalho.largura))
	{
		imagem_branca(pixel, sola, pixeisQuantidade);

		ymeio = xmeio = 0;
		anguloCos = cos(PI);
		anguloSin = sin(PI);
		xtemp = (int)((cabecalho.largura / 2) - (anguloCos * (cabecalho.largura / 2) - (anguloSin * (cabecalho.altura / 2))));
		ytemp = (int)((cabecalho.altura / 2) - (anguloSin * (cabecalho.largura / 2) + anguloCos * (cabecalho.altura / 2)));

		rotaciona_e_realoca(pixel, sola, xmeio, ymeio, anguloCos, anguloSin, xtemp, ytemp, pixeisQuantidade);
	}

	dilatacao(cabecalho.altura, cabecalho.largura, pixel, sola, pixeisQuantidade);

	libera_matriz_sola_XY(sola);
	libera_matriz_sola_XY(estremidade);

	escreve_na_imagem(pixel, cabecalho.altura, cabecalho.largura, saida, cabecalho);

	libera_matriz_imagem(pixel, cabecalho.altura);

	return;
}