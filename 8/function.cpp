#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "header.h"

typedef	unsigned char	uchar;

//----------------------------------------------------------------
//  Function-ID	rounding(double dv)　ダブル型値を0-255の範囲に丸める
//----------------------------------------------------------------
unsigned char rounding(double dv)
{
    unsigned char ucv; 
	int iv = (int)(dv + 0.5);
	if (iv < 0) {
		iv=0;
	}
	else if (iv > 255) {
		iv=255;
	}
    ucv = (unsigned char)iv;
    return ucv;
}

//----------------------------------------------------------------
//  Function-ID	rounding_integer(int idv) 整数値を0-255の範囲に丸める
//----------------------------------------------------------------
unsigned char rounding_integer(int idv)
{
    unsigned char ucv; 
	int iv;
	iv=idv;
	if (idv < 0){
		iv=0;
	}
	else if (idv > 255){
		iv=255;
	}
    ucv = (unsigned char)iv;
    return ucv;
}

//------------------------------------------------------------------------------
//	Function-ID	m_alloc
//	Function-Name	メモリ確保
//	Abstract	異常終了時にエラーメッセージを返すmalloc関数
//	Argument	int	size	= 確保するメモリサイズ
//	Return-Value	void	*a	= 確保したメモリのアドレス
//	Special_Desc
//------------------------------------------------------------------------------
void *
m_alloc(int	size)
{
    void	*a;

    if ((a = malloc(size)) == NULL) {
	fprintf(stderr, "m_alloc: memory allocation error.\n");
	exit(-1);
    }

    return (a);
}

//----------------------------------------------------------------
//	Function-ID	alloc_IMG_YUV
//	Function-Name	IMGデータ領域確保
//	Abstract	IMG_YUVデータのメモリを確保する
//	Argument	int	width	= 画像横サイズ
//				int	height	= 画像縦サイズ
//	Return-Value	IMG_YUV *		= 確保した領域のポインタ
//	Special_Desc	
//----------------------------------------------------------------
IMG_YUV *
alloc_IMG_YUV(int	width,  int	height)
{
    IMG_YUV	*img;

    img		= (IMG_YUV   *) m_alloc(sizeof (IMG_YUV));
    img->width	= width;
    img->height	= height;
    img->pixel	= img->width * img->height;
    img->Y	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->U	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->V	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);

    return (img);
}

//----------------------------------------------------------------
//	Function-ID	alloc_IMG_RGB
//	Function-Name	IMGデータ領域確保
//	Abstract	IMG_RGBデータのメモリを確保する
//	Argument	int	width	= 画像横サイズ
//				int	height	= 画像縦サイズ
//	Return-Value	IMG_RGB *		= 確保した領域のポインタ
//	Special_Desc	
//----------------------------------------------------------------
IMG_RGB *
alloc_IMG_RGB(int	width,  int	height)
{
    IMG_RGB	*img;

    img		= (IMG_RGB   *) m_alloc(sizeof (IMG_RGB));
    img->width	= width;
    img->height	= height;
    img->pixel	= img->width * img->height;
    img->R	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->G	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);
	img->B	= (uchar *) m_alloc(sizeof (uchar) * img->pixel);

    return (img);
}

//------------------------------------------------------------------------------
//	Function-ID	free_IMG_YUV
//	Function-Name	IMG_YUVデータメモリ解放
//	Abstract	IMG_YUVデータの領域を解放する
//	Argument	IMG_YUV	*img	= 画像構造体
//	Return-Value	Nothing
//	Special_Desc	
//------------------------------------------------------------------------------
void
free_IMG_YUV(IMG_YUV	*img)
{
    if (img->Y)		free(img->Y);
    if (img->U)		free(img->U);
    if (img->V)		free(img->V);
    free(img);
}

//------------------------------------------------------------------------------
//	Function-ID	free_IMG_RGB
//	Function-Name	IMG_RGBデータメモリ解放
//	Abstract	IMG_RGBデータの領域を解放する
//	Argument	IMG_RGB	*img	= 画像構造体
//	Return-Value	Nothing
//	Special_Desc	
//------------------------------------------------------------------------------
void
free_IMG_RGB(IMG_RGB	*img)
{
    if (img->R)		free(img->R);
    if (img->G)		free(img->G);
    if (img->B)		free(img->B);
    free(img);
}


//----------------------------------------------------------------
//	Function-ID	alloc_IMG_YUV_DOUBLE
//	Function-Name	IMGデータ領域確保
//	Abstract	IMG_YUV_DOUBLEデータのメモリを確保する
//	Argument	int	width	= 画像横サイズ
//				int	height	= 画像縦サイズ
//	Return-Value	IMG_YUV_DOUBLE *		= 確保した領域のポインタ
//	Special_Desc	
//----------------------------------------------------------------
IMG_YUV_DOUBLE *
alloc_IMG_YUV_DOUBLE(int	width, int	height)
{
	IMG_YUV_DOUBLE	*img_double;

	img_double = (IMG_YUV_DOUBLE   *)m_alloc(sizeof (IMG_YUV_DOUBLE));
	img_double->width = width;
	img_double->height = height;
	img_double->pixel = img_double->width * img_double->height;
	img_double->Y = (double *)m_alloc(sizeof (double)* img_double->pixel);
	img_double->U = (double *)m_alloc(sizeof (double)* img_double->pixel);
	img_double->V = (double *)m_alloc(sizeof (double)* img_double->pixel);

	return (img_double);
}


//------------------------------------------------------------------------------
//	Function-ID	free_IMG_YUV_DOUBLE
//	Function-Name	IMG_YUV_DOUBLEデータメモリ解放
//	Abstract	IMG_YUV_DOUBLEデータの領域を解放する
//	Argument	IMG_YUV_DOUBLE	*img_double	= 画像構造体
//	Return-Value	Nothing
//	Special_Desc	
//------------------------------------------------------------------------------
void
free_IMG_YUV_DOUBLE(IMG_YUV_DOUBLE	*img_double)
{
	if (img_double->Y)		free(img_double->Y);
	if (img_double->U)		free(img_double->U);
	if (img_double->V)		free(img_double->V);
	free(img_double);
}

//----------------------------------------------------------------
//	Function-ID	read_bmp
//	Function-Name	ビットマップ読み込み
//	Abstract	img_rgbに読み込み、 YUV変換してimgに格納
//	Argument	IMG_YUV *img:	YUV信号を格納する領域
//              IMG_RGB *img_rgb:	読み込んだBMPを格納する領域
//              FILE *infile:       読み込み先ポインタ
//	Special_Desc	24ビットのみ対応
//
//            +---------------------------------
//               Coded by Takashi Yoshino
//               Revised by Y.Yashima 2012.09.15
//----------------------------------------------------------------
void read_bmp(IMG_YUV *img,IMG_RGB *img_rgb, FILE *infile){
	int width, height;
	int size;

	width = img->width;
    height = img->height;
	size = width * height;
	int k,j;
	double y,u,v;
//	double r,g,b;

	//画像データを読み込む
	for( k=height-1; k>=0; k--){
		for( j=0; j<width; j++){
			img_rgb->B[j+k*width] = getc(infile);
			img_rgb->G[j+k*width] = getc(infile);
			img_rgb->R[j+k*width] = getc(infile);
		}
	}

	for(int k=0;k<height;k++){
		for(int j=0;j<width;j++){
				y = 0.2126 * img_rgb->R[ j+k*width ] + 0.7152 * img_rgb->G[ j+k*width ] + 0.0722 * img_rgb->B[ j+k*width ];
				u = -0.1146 * img_rgb->R[ j+k*width ] - 0.3854 * img_rgb->G[ j+k*width ] + 0.5 * img_rgb->B[ j+k*width ];
				v = 0.5 * img_rgb->R[ j+k*width ] - 0.4542 * img_rgb->G[ j+k*width ] - 0.0458 * img_rgb->B[ j+k*width ];
				img->Y[ j + k*width ] = rounding(y);       /* Y conponent */
				img->U[ j + k*width ] = rounding(u+128);   /* U conponent */
				img->V[ j + k*width ] = rounding(v+128);   /* V conponent */
		}
	}
}
//----------------------------------------------------------------
//	Function-ID	write_bmp
//	Function-Name	ビットマップ書き込み
//	Abstract	YUVをRGB変換してimg_rbg2に格納
//	Argument	IMG_YUV *img:	YUV信号が格納されている画像データ
//              IMG_RGB *img_rgb:       色変換後の画像格納領域（作業用配列）   
//              FILE *outfile:       書き込み先ポインタ
//	Special_Desc	24ビットのみ対応
//
//            +---------------------------------
//               Coded by Takashi Yoshino
//               Revised by Y.Yashima 2012.09.15
//----------------------------------------------------------------
void write_bmp(IMG_YUV *img, IMG_RGB *img_rgb, FILE *outfile){
    int width, height;
	int size;

	width = img->width;
    height = img->height;
	size = width * height;
	int k,j;
	double r,g,b;

	  	//YUV-RGB変換と並べ替え
		for(k=0;k<height;k++){
			for(j=0; j<width; j++){
			  	r = img->Y[ j + k*width ] + 1.5748 * (img->V[ j + k*width ]-128);
				g = img->Y[ j + k*width ] - 0.1873 * (img->U[ j + k*width ]-128) - 0.4681 * (img->V[ j + k*width ]-128);
				b = img->Y[ j + k*width ] + 1.8556 * (img->U[ j + k*width ]-128) ;
				img_rgb->R[ j + (height-k-1)*width ] = rounding(r);
				img_rgb->G[ j + (height-k-1)*width ] = rounding(g);
				img_rgb->B[ j + (height-k-1)*width ] = rounding(b);
			}
		}

		 //ヘッダ書き込み
	  uchar header_buf[54];
	  unsigned int file_size;
	  unsigned int offset_to_data;
	  unsigned long info_header_size;
	  unsigned int planes;
	  unsigned int color;
	  unsigned long compress;
	  unsigned long data_size;
	  long xppm;
	  long yppm;

	  file_size=size+54;
	  offset_to_data=54;
	  info_header_size=40;
	  planes = 1;
	  color = 24;
	  compress = 0;
	  data_size = size;
	  xppm = 1;
	  yppm = 1;

	  header_buf[0]='B';
	  header_buf[1]='M';
	  memcpy(header_buf+2,&file_size,sizeof(file_size));
	  header_buf[6]=0;
	  header_buf[7]=0;
	  header_buf[8]=0;
	  header_buf[9]=0;
	  memcpy(header_buf + 10, &offset_to_data, sizeof(file_size));
	  header_buf[11]=0;
	  header_buf[12]=0;
	  header_buf[13]=0;
	  memcpy(header_buf + 14, &info_header_size, sizeof(info_header_size));
	  header_buf[15] = 0;
	  header_buf[16] = 0;
	  header_buf[17] = 0;
	  memcpy(header_buf + 18, &width, sizeof(width));
	  memcpy(header_buf + 22, &height, sizeof(height));
	  memcpy(header_buf + 26, &planes, sizeof(planes));
	  memcpy(header_buf + 28, &color, sizeof(color));
	  memcpy(header_buf + 30, &compress, sizeof(compress));
	  memcpy(header_buf + 34, &data_size, sizeof(data_size));
	  memcpy(header_buf + 38, &xppm, sizeof(xppm));
	  memcpy(header_buf + 42, &yppm, sizeof(yppm));
	  header_buf[46] = 0;
	  header_buf[47] = 0;
	  header_buf[48] = 0;
	  header_buf[49] = 0;
	  header_buf[50] = 0;
	  header_buf[51] = 0;
	  header_buf[52] = 0;
	  header_buf[53] = 0;

	  fwrite(header_buf, sizeof(unsigned char), 54, outfile);

	  for(int kk=0;kk<size;kk++){
		  fputc(img_rgb->B[kk],outfile);		//Bを出力
		  fputc(img_rgb->G[kk],outfile);		//G
		  fputc(img_rgb->R[kk],outfile);		//R
	  }
}

//----------------------------------------------------------------
//	Function-ID	copy_image
//	Function-Name	
//	Abstract	IMG_YUV型構造体の画像データをコピーする（構造体ポインタを引き渡す）
//
//	Argument	IMG_YUV *img1: copy元画像データ
//		        IMG_YUV *img2: copy先画像データ
//		        int param : param=1(0以外)のときYUVすべてをコピー（カラーのままコピー）
//                          param=0のときYだけコピーしてUとVには128を入れる（モノクロ化）
//			
//	Return-Value	
//	Special_Desc
//
//            +---------------------------------
//               Coded by Y.Yashima
//
//----------------------------------------------------------------
void  copy_image(IMG_YUV *img1, IMG_YUV *img2, int param)
{

	 int j, k; 
	 int width, height;

	 width = img1->width;
     height = img1->height;

	 if(param==0) {
		 for(k=0;k<height;k++){
			 for(j=0; j<width; j++){
				 img2->Y[ j + k*width ] = img1->Y[ j + k*width ];
				 img2->U[ j + k*width ] = 128;
				 img2->V[ j + k*width ] = 128;
			 }
		 }
	 }
	 else{
		 for(k=0;k<height;k++){
			 for(j=0; j<width; j++){
				 img2->Y[ j + k*width ] = img1->Y[ j + k*width ];
				 img2->U[ j + k*width ] = img1->U[ j + k*width ];
				 img2->V[ j + k*width ] = img1->V[ j + k*width ];
			 }
		 }
	 }

}

//----------------------------------------------------------------
//	Function-ID	setvalue_image
//	Function-Name	
//	Abstract	IMG_YUV型構造体の画像データの各コンポーネントに定められた値をセットする
//
//	Argument	IMG_YUV *img1: 画像データ
//		        int Yvalue : 輝度信号にセットする値（0-255）
//		        int Uvalue : 輝度信号にセットする値（0-255）
//		        int Vvalue : 輝度信号にセットする値（0-255）
//			
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void  setvalue_image(IMG_YUV *img1, int Yvalue, int Uvalue, int Vvalue)
{

	int j, k; 
	int width, height;
	unsigned char Ydata, Udata, Vdata;

	width = img1->width;
    height = img1->height;

	if(Yvalue<0) {
		Ydata = 0;
	}
	else if(Yvalue>255){
		Ydata = 255;
	}
	else{
		Ydata = (unsigned char) Yvalue;
	}

	if(Uvalue<0) {
		Udata = 0;
	}
	else if(Uvalue>255){
		Udata = 255;
	}
	else{
		Udata = (unsigned char) Uvalue;
	}

	if(Vvalue<0) {
		Vdata = 0;
	}
	else if(Vvalue>255){
		Vdata = 255;
	}
	else{
		Vdata = (unsigned char) Vvalue;
	}

	for(k=0;k<height;k++){
		for(j=0; j<width; j++){
			img1->Y[ j + k*width ] = Ydata;
	    }
	}

	for(k=0;k<height;k++){
		for(j=0; j<width; j++){
			img1->U[ j + k*width ] = Udata;
		    img1->V[ j + k*width ] = Vdata;
		}
	}

}

//----------------------------------------------------------------
//	Function-ID	dft_image
//	Function-Name	
//	Abstract	画像の２次元フーリエ変換を行う
//                (Yコンポーネントについて計算）
//
//	Argument	IMG_YUV *img: 処理対象画像データ
//		        IMG_YUV_DOUBLE *imgr: 作業用配列（実部）
//		        IMG_YUV_DOUBLE *imgi: 作業用配列（虚部）
//		        IMG_YUV_DOUBLE *imgwr: 作業用配列（実部）
//		        IMG_YUV_DOUBLE *imgwi: 作業用配列（虚部）
//		        IMG_YUV_DOUBLE *imgfr: 変換係数（DFT係数，実部）
//		        IMG_YUV_DOUBLE *imgfi: 変換係数（DFT係数，虚部）
//		        IMG_YUV_DOUBLE *imgpower: 周波数パワー
//			                    
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void dft_image(IMG_YUV *img, IMG_YUV_DOUBLE *imgr, IMG_YUV_DOUBLE *imgi,
	IMG_YUV_DOUBLE *imgwr, IMG_YUV_DOUBLE *imgwi, IMG_YUV_DOUBLE *imgfr, IMG_YUV_DOUBLE *imgfi,
	IMG_YUV_DOUBLE *imgpower)
{

	int j, k, u, v;
	int width, height;
	double sumr, sumi;

	width = img->width;
	height = img->height;

	/* For Y-signal ============================= */

	for (k = 0; k < height; k++){
		for (j = 0; j < width; j++){
			imgr->Y[j + k*width] = img->Y[j + k*width];
			imgi->Y[j + k*width] = 0;
		}
	}

	/* 水平方向（j方向）の1次元DFT */
	printf(" == Horizontal 1-D DFT for Y-signal \n");
	for (k = 0; k<height; k++){
		if((k%50)==0) printf("  processing line No. %d \n", k);
		for (u = 0; u<width; u++){
			sumr = 0;
			sumi = 0;
			for (j = 0; j<width; j++){
				sumr = sumr + imgr->Y[j + k*width] * cos(2.0*PI*j*u / (double)width) + imgi->Y[j + k*width] * sin(2.0*PI*j*u / (double)width);
				sumi = sumi + imgi->Y[j + k*width] * cos(2.0*PI*j*u / (double)width) - imgr->Y[j + k*width] * sin(2.0*PI*j*u / (double)width);
			}
			imgwr->Y[u + k*width] = sumr / width;
			imgwi->Y[u + k*width] = sumi / width;
		}
	}
	/* 垂直方向（k方向）の1次元DFT */
	printf(" == Vertical 1-D DFT for Y-signal \n");
	for (u = 0; u<width; u++){
		if ((u % 50) == 0) printf("  processing column No. %d \n", u);
		for (v = 0; v<height; v++){
			sumr = 0;
			sumi = 0;
			for (k = 0; k<height; k++){
				sumr = sumr + imgwr->Y[u + k*width] * cos(2.0*PI*k*v / (double)height) + imgwi->Y[u + k*width] * sin(2.0*PI*k*v / (double)height);
				sumi = sumi + imgwi->Y[u + k*width] * cos(2.0*PI*k*v / (double)height) - imgwr->Y[u + k*width] * sin(2.0*PI*k*v / (double)height);
			}
			imgfr->Y[u + v*width] = sumr / height;
			imgfi->Y[u + v*width] = sumi / height;
		}
	}

	/* DFT power calculation */
	for (v = 0; v < height; v++){
		for (u = 0; u < width; u++){
			imgpower->Y[u + v*width] = sqrt(imgfr->Y[u + v*width] * imgfr->Y[u + v*width] + imgfi->Y[u + v*width] * imgfi->Y[u + v*width]);
		}
	}

}

//----------------------------------------------------------------
//	Function-ID	double2uchar_image
//	Function-Name	
//	Abstract	double型画像をunsigned char型整数表現に変換する
//                Y信号のみ処理．U/Vには128が入れられる．
//
//	Argument	IMG_YUV_DOUBLE *img: 処理対象画像データ
//		        IMG_YUV *imgout: 出力画像
//		        int param: 変換パラメータ
//                            param=1: 定数倍して0-255にクリップ
//                            param=2: 定数倍して絶対値をとり，0-255にクリップ
//                            param=3: 定数倍して+128だけシフトした後に，0-255にクリップ（グレーをゼロレベルとする）
//		        double cvalue: 定数倍する値
//			                    
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void double2uchar_image(IMG_YUV_DOUBLE *img, IMG_YUV *imgout, int param, double cvalue)
{

	int j, k;
	int width, height, widthc, heightc;

	width = img->width;
	height = img->height;
	widthc = width;
	heightc = height;

	if (param == 2){
		for (k = 0; k < height; k++){
			for (j = 0; j < width; j++){
				imgout->Y[j + k*width] = rounding(fabs(img->Y[j + k*width] * cvalue));
			}
		}
	}
	else if (param == 3){
		for (k = 0; k < height; k++){
			for (j = 0; j < width; j++){
				imgout->Y[j + k*width] = rounding(img->Y[j + k*width] * cvalue + 128);
			}
		}
	}
	else{   /* param = 1 */
		for (k = 0; k < height; k++){
			for (j = 0; j < width; j++){
				imgout->Y[j + k*width] = rounding(img->Y[j + k*width] * cvalue);
			}
		}
	}

	/* set to 128 for U/V-signal */
	for (k = 0; k < heightc; k++){
		for (j = 0; j < widthc; j++){
			imgout->U[j + k*widthc] = 128;
			imgout->V[j + k*widthc] = 128;
		}
	}

}

//----------------------------------------------------------------
//	Function-ID	dft_image_shift
//	Function-Name	
//	Abstract	フーリエ変換係数の並び替え（中央が直流成分になるように）
//
//	Argument	IMG_YUV *imgfp: フーリエ変換係数パワー
//		        IMG_YUV *imgfps: 並べ替えられたフーリエ係数パワー
//			                    
//	Return-Value	
//	Special_Desc	
//----------------------------------------------------------------
void dft_image_shift(IMG_YUV *imgfp, IMG_YUV *imgfps)
{

	int u, v;
	int width, height, widthc, heightc;
	int w2, h2, wc2, hc2;

	width = imgfp->width;
	height = imgfp->height;
	widthc = width;
	heightc = height;
	w2 = width / 2;
	h2 = height / 2;
	wc2 = widthc / 2;
	hc2 = heightc / 2;

	for (v = 0; v < h2; v++){
		for (u = 0; u < w2; u++){
			imgfps->Y[u + v*width] = imgfp->Y[u + w2 + (v + h2)*width];
			imgfps->Y[u + w2 + v*width] = imgfp->Y[u + (v + h2)*width];
			imgfps->Y[u + (v + h2)*width] = imgfp->Y[u + w2 + v*width];
			imgfps->Y[u + w2 + (v + h2)*width] = imgfp->Y[u + v*width];
		}
	}

	for (v = 0; v < hc2; v++){
		for (u = 0; u < wc2; u++){
			imgfps->U[u + v*widthc] = imgfp->U[u + wc2 + (v + hc2)*widthc];
			imgfps->U[u + wc2 + v*widthc] = imgfp->U[u + (v + hc2)*widthc];
			imgfps->U[u + (v + hc2)*widthc] = imgfp->U[u + wc2 + v*widthc];
			imgfps->U[u + wc2 + (v + hc2)*widthc] = imgfp->U[u + v*widthc];
			imgfps->V[u + v*widthc] = imgfp->V[u + wc2 + (v + hc2)*widthc];
			imgfps->V[u + wc2 + v*widthc] = imgfp->V[u + (v + hc2)*widthc];
			imgfps->V[u + (v + hc2)*widthc] = imgfp->V[u + wc2 + v*widthc];
			imgfps->V[u + wc2 + (v + hc2)*widthc] = imgfp->V[u + v*widthc];
		}
	}

}

void create_number_line(uchar(*array_line)[16], int student_number)
{
	int number, number1;
	int jj, kk, jst;
	uchar numarray[16][16];

	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<176; jj++){
			array_line[jj][kk] = 0;
		}
	}

	jst = 8;
	number = student_number / 1000000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = student_number - number * 1000000;
	number = number1 / 100000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 100000;
	number = number1 / 10000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 10000;
	number = number1 / 1000;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 1000;
	number = number1 / 100;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 100;
	number = number1 / 10;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

	jst = jst + 24;
	number1 = number1 - number * 10;
	number = number1;
	create_number(numarray, number);
	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			array_line[jst + jj][kk] = numarray[jj][kk];
		}
	}

}

void create_number(uchar(*numarray)[16], int number)
{
	int jj, kk;

	for (kk = 0; kk<16; kk++){
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 0;
		}
	}

	if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){
		/* pettern-1 */
		kk = 0;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-2 */
		jj = 0;
		for (kk = 0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 1 || number == 2 || number == 3 || number == 4 || number == 7 || number == 8 || number == 9){
		/* pettern-3 */
		jj = 15;
		for (kk = 0; kk<8; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-4 */
		kk = 7;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 2 || number == 6 || number == 8){
		/* pettern-5 */
		jj = 0;
		for (kk = 8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 1 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9){
		/* pettern-6 */
		jj = 15;
		for (kk = 8; kk<15; kk++){
			numarray[jj][kk] = 255;
		}
	}

	if (number == 0 || number == 2 || number == 3 || number == 5 || number == 6 || number == 8 || number == 9){
		/* pettern-7 */
		kk = 15;
		for (jj = 0; jj<16; jj++){
			numarray[jj][kk] = 255;
		}
	}

}

