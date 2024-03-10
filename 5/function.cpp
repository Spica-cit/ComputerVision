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

//----------------------------------------------------------------
//  Function-ID	rounding_off(double x) 実数の小数点以下を四捨五入する
//----------------------------------------------------------------
int rounding_off(double x)
{
	return (int)(x < 0.0 ? x - 0.5 : x + 0.5);
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

//------------------------------------------------------------------------------
//	Function-ID	bicubic_sinc_function
//	Function-Name	
//	Abstract	バイキュービック用sinc関数
//	Argument	double t	= 中心からの距離
//	Return-Value	double	value = h(t)
//	Special_Desc
//------------------------------------------------------------------------------
double bicubic_sinc_function(double	t)
{
	double	value, at;

	at = fabs(t);
	if (at <= 1){
		value = 1 - 2 * t*t + t*t*t;
	}
	else if (at>2){
		value = 0;
	}
	else{
		value = 4 - 8 * t + 5 * t*t - t*t*t;
	}

	return (value);
}

//----------------------------------------------------------------
//	Function-ID	affine4_image
//	Function-Name	
//	Abstract	画像のアフィン変換を行う．
//              6パラメータの行列による変換．
//
//	Argument	IMG_YUV *img: 処理対象画像データ
//		        IMG_YUV *imgout: アフィン変換後の画像データ
//                  画像の中心（width/2, height/2)を(0,0)とみなして幾何変換を行う
//
//              double a,b,c,d,e,f: アフィン変換パラメータ
//
//                 X = ax+by+e
//                 Y = cx+dy+f
//			                    
//	Return-Value	
//	Special_Desc	This function calls "bicubic_sinc_function"
//----------------------------------------------------------------
void  affine4_image(IMG_YUV *img, IMG_YUV *imgout, double a, double b, double c, double d, double e, double f)
{

	int j, k;
	int jj, kk;
	int width, height, widthc, heightc;
	int w2, h2, w2c, h2c;
	double Det;
	double jf, kf;
	int j0, k0;
	int loop, loopj, loopk, loopw;
	double d0x[4], d0y[4];
	int fY[4][4], fU[4][4], fV[4][4];
	double hy[4], hx[4];
	double sum, work[4];
	double sumU, sumV, workU[4], workV[4];

	width = img->width;
	height = img->height;
	widthc = width;
	heightc = height;
	w2 = width / 2;
	h2 = height / 2;
	w2c = widthc / 2;
	h2c = heightc / 2;

	/* === Y-signal processing ========================= */
	//	printf("affine_image: Y-signal processing \n");
	for (k = 0; k<height; k++){       /* 変換後の画像上でスキャン */
		for (j = 0; j<width; j++){    /* 変換後の画像上でスキャン */

			/* 回転前の座標値に変換 (in case of 0<r<90) */

			Det = a*d - b*c;
			jf = (d*(j - w2 - e) - b*(k - h2 - f)) / Det + w2;
			kf = (-c*(j - w2 - e) + a*(k - h2 - f)) / Det + h2;

			jf = rounding_off(jf*100000.0) / 100000.0;   /* 演算誤差修正のための四捨五入 */
			kf = rounding_off(kf*100000.0) / 100000.0;   /* 演算誤差修正のための四捨五入 */

			if (jf<0 || jf>(width - 1) || kf<0 || kf>(height - 1)) continue; /* 処理対象領域外 */

			/* --- by bicubic interpolation ----------------------*/

			k0 = (int)kf;   // 補間座標に対して左上に存在する入力画像の画素位置座標
			d0y[1] = kf - k0; d0y[0] = 1 + kf - k0; d0y[2] = 1 - (kf - k0); d0y[3] = 2 - (kf - k0);   // 4つの実在画素位置座標までの距離（垂直）
			for (loop = 0; loop <= 3; loop++){
				hy[loop] = bicubic_sinc_function(d0y[loop]);
			}

			j0 = (int)jf;   // 補間座標に対して左上に存在する入力画像の画素位置座標
			d0x[1] = jf - j0; d0x[0] = 1 + jf - j0; d0x[2] = 1 - (jf - j0); d0x[3] = 2 - (jf - j0);   // 4つの実在画素位置座標までの距離（水平）
			for (loop = 0; loop <= 3; loop++){
				hx[loop] = bicubic_sinc_function(d0x[loop]);
			}

			for (loopk = -1; loopk <= 2; loopk++){
				kk = k0 + loopk;
				if (kk<0) { kk = 0; }
				else if (kk>(height - 1)) { kk = height - 1; }
				for (loopj = -1; loopj <= 2; loopj++){
					jj = j0 + loopj;
					if (jj<0) { jj = 0; }
					else if (jj>(width - 1)) { jj = width - 1; }
					fY[loopj + 1][loopk + 1] = img->Y[jj + kk*width];    /* 補間に使う画素を4×4行列fY[4][4]に代入 */
				}
			}
			for (loopw = 0; loopw <= 3; loopw++){
				sum = 0;
				for (loop = 0; loop <= 3; loop++){
					sum = sum + hy[loop] * fY[loopw][loop];
				}
				work[loopw] = sum;
			}
			sum = 0;
			for (loop = 0; loop <= 3; loop++){
				sum = sum + work[loop] * hx[loop];
			}
			imgout->Y[j + k*width] = rounding(sum);
		}
	}

	/* === U,V-signal processing ========================= */
	//	printf("affine_image: UV-signal processing \n");

	for (k = 0; k<heightc; k++){       /* 変換後の画像上でスキャン */
		for (j = 0; j<widthc; j++){    /* 変換後の画像上でスキャン */

			/* 変換前の座標値に変換 */

			Det = a*d - b*c;
			jf = (d*(j - w2c - e) - b*(k - h2c - f)) / Det + w2c;
			kf = (-c*(j - w2c - e) + a*(k - h2c - f)) / Det + h2c;

			jf = rounding_off(jf*100000.0) / 100000.0;   /* 演算誤差修正のための四捨五入 */
			kf = rounding_off(kf*100000.0) / 100000.0;   /* 演算誤差修正のための四捨五入 */

			if (jf<0 || jf>(widthc - 1) || kf<0 || kf>(heightc - 1)) continue; /* 処理対象領域外 */

			/* --- by bicubic interpolation ----------------------*/

			k0 = (int)kf;   // 補間座標に対して左上に存在する入力画像の画素位置座標
			d0y[1] = kf - k0; d0y[0] = 1 + kf - k0; d0y[2] = 1 - (kf - k0); d0y[3] = 2 - (kf - k0);   // 4つの実在画素位置座標までの距離（垂直）
			for (loop = 0; loop <= 3; loop++){
				hy[loop] = bicubic_sinc_function(d0y[loop]);
			}

			j0 = (int)jf;   // 補間座標に対して左上に存在する入力画像の画素位置座標
			d0x[1] = jf - j0; d0x[0] = 1 + jf - j0; d0x[2] = 1 - (jf - j0); d0x[3] = 2 - (jf - j0);   // 4つの実在画素位置座標までの距離（水平）
			for (loop = 0; loop <= 3; loop++){
				hx[loop] = bicubic_sinc_function(d0x[loop]);
			}

			for (loopk = -1; loopk <= 2; loopk++){
				kk = k0 + loopk;
				if (kk<0) { kk = 0; }
				else if (kk>(heightc - 1)) { kk = heightc - 1; }
				for (loopj = -1; loopj <= 2; loopj++){
					jj = j0 + loopj;
					if (jj<0) { jj = 0; }
					else if (jj>(widthc - 1)) { jj = widthc - 1; }
					fU[loopj + 1][loopk + 1] = img->U[jj + kk*widthc];    /* 補間に使う画素を4×4行列f[4][4]に代入 */
					fV[loopj + 1][loopk + 1] = img->V[jj + kk*widthc];    /* 補間に使う画素を4×4行列f[4][4]に代入 */
				}
			}
			for (loopw = 0; loopw <= 3; loopw++){
				sumU = 0;
				sumV = 0;
				for (loop = 0; loop <= 3; loop++){
					sumU = sumU + hy[loop] * fU[loopw][loop];
					sumV = sumV + hy[loop] * fV[loopw][loop];
				}
				workU[loopw] = sumU;
				workV[loopw] = sumV;
			}
			sumU = 0;
			sumV = 0;
			for (loop = 0; loop <= 3; loop++){
				sumU = sumU + workU[loop] * hx[loop];
				sumV = sumV + workV[loop] * hx[loop];
			}
			imgout->U[j + k*widthc] = rounding(sumU);
			imgout->V[j + k*widthc] = rounding(sumV);

		}
	}

}

//-----------------------------------------------------------------------------
//	Function-ID	draw_line
//	Function-Name	
//	Abstract	直線（水平および垂直）を描画する
//
//	Argument	IMG_YUV *img: 描画する画像データ（入出力，444形式であること）
//		        int xs : 始点水平座標
//		        int ys : 始点垂直座標
//		        int xe : 終点水平座標
//		        int ye : 終点垂直座標
//		        int color : 描画される線の色
//                            （-1:黒，0:白，1:黄緑，2:青，3:赤，4:ピンク，5:黄）
//			
//	Return-Value	
//	Special_Desc	
//
//-----------------------------------------------------------------------------
void  draw_line(IMG_YUV *img, int xs, int ys, int xe, int ye, int color)
{

	int j, k;
	int width, height;
	int widthc, heightc;
	int dataY, dataU, dataV;

	width = img->width;
	height = img->height;
	widthc = width;
	heightc = height;

	if (color == -1) { dataY = 0; dataU = 128; dataV = 128; }   /* 黒 */
	if (color == 0) { dataY = 255; dataU = 128; dataV = 128; }   /* 白 */
	if (color == 1) { dataY = 128; dataU = 0; dataV = 0; }   /* 黄緑 */
	if (color == 2) { dataY = 128; dataU = 255; dataV = 0; }   /* 青 */
	if (color == 3) { dataY = 128; dataU = 0; dataV = 255; }   /* 赤 */
	if (color == 4) { dataY = 128; dataU = 255; dataV = 255; }   /* ピンク */
	if (color == 5) { dataY = 255; dataU = 0; dataV = 128; }   /* 黄 */

	for (j = xs; j <= xe; j++){
		img->Y[j + ys*width] = dataY;
		img->U[j + ys*width] = dataU;
		img->V[j + ys*width] = dataV;
		img->Y[j + ye*width] = dataY;
		img->U[j + ye*width] = dataU;
		img->V[j + ye*width] = dataV;
	}
	for (k = ys; k <= ye; k++){
		img->Y[xs + k*width] = dataY;
		img->U[xs + k*width] = dataU;
		img->V[xs + k*width] = dataV;
		img->Y[xe + k*width] = dataY;
		img->U[xe + k*width] = dataU;
		img->V[xe + k*width] = dataV;
	}

}

//-----------------------------------------------------------------------------
//	Function-ID	draw_axis
//	Function-Name	
//	Abstract	座標軸を描画する
//
//	Argument	IMG_YUV *img: 描画する画像データ（入出力，444形式であること）
//		        int xs : 始点水平座標
//		        int ys : 始点垂直座標
//		        int xe : 終点水平座標
//		        int ye : 終点垂直座標
//		        int color : 描画される線の色
//                            （-1:黒，0:白，1:黄緑，2:青，3:赤，4:ピンク，5:黄）
//			
//	Return-Value	
//	Special_Desc	
//
//-----------------------------------------------------------------------------
void  draw_axix(IMG_YUV *img, int jc, int kc)
{

	int width, height;
	int color;
	int x, y, xs, ys, xe, ye;

	width = img->width;
	height = img->height;

	/* 座標軸の描画 */
	color = 2;
	ys = 0;
	ye = height - 1;
	for (x = jc; x<width; x = x + 100){
		xs = x;
		xe = x;
		draw_line(img, xs, ys, xe, ye, color);
	}
	for (x = jc; 0 <= x; x = x - 100){
		xs = x;
		xe = x;
		draw_line(img, xs, ys, xe, ye, color);
	}
	xs = 0;
	xe = width - 1;
	for (y = kc; y<height; y = y + 100){
		ys = y;
		ye = y;
		draw_line(img, xs, ys, xe, ye, color);
	}
	for (y = kc; 0 <= y; y = y - 100){
		ys = y;
		ye = y;
		draw_line(img, xs, ys, xe, ye, color);
	}

	color = 3;
	draw_line(img, jc, 0, jc, height - 1, color);
	draw_line(img, jc - 1, 0, jc - 1, height - 1, color);
	draw_line(img, jc + 1, 0, jc + 1, height - 1, color);
	draw_line(img, 0, kc, width - 1, kc, color);
	draw_line(img, 0, kc - 1, width - 1, kc - 1, color);
	draw_line(img, 0, kc + 1, width - 1, kc + 1, color);

}
