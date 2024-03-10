#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size;
	 int height,width;
	 int k,j;
	 double a,b,value;
	 int number;

	 IMG_YUV *img, *img_out;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

     /* パラメータチェック */
     if (argc < 6) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infile outfile a b student number \n", argv[0]);
	     printf("        infile: input file name \n");
	     printf("        outfile: output file name \n");
		 printf("        a(double): conversion parameter (a for (ax+b)) \n");
		 printf("        b(double): conversion parameter (b for (ax+b)) \n");
		 printf("        number: student number \n");
		 exit(-1);
     }

	 /* パラメータ設定 */
	 strcpy(infname1, argv[1]);      /* 入力画像ファイル名 */
     strcpy(outfname1, argv[2]);     /* 出力画像ファイル名 */
	 a = atof(argv[3]);         /* 階調変換パラメータ */
	 b = atof(argv[4]);         /* 階調変換パラメータ */
	 number = atoi(argv[5]);      /* 学生番号(7桁） */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file = %s \n", infname1);
     printf("   -- Output file = %s \n", outfname1);
     printf("   -- a = %f \n", a);
	 printf("   -- b = %f \n", b);
	 printf("   -- Student number= %d \n", number);

	 /* 画像ファイルのオープン */
     if( !strcmp(infname1,"-") ){
		 infile = stdin;
     }
     else if ((infile = fopen(infname1, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname1);
		 exit(-1);
     }

	 sprintf(outfname1, "%s", argv[2]);
     if ((outfile = fopen(outfname1, "wb")) == NULL) {
		 fprintf(stderr, "OutPut File open error = %s\n", outfname1);
		 exit(-1);
     }

	//ビットマップファイルのヘッダ部を読み込み
	for(int c=0; c<54; c++){
		head[c] = fgetc(infile);
	}

	//ヘッダからの幅と高さを算出
	width  = head[18] | head[19]<<8 | head[20]<<16 | head[21]<<24;
	height = head[22] | head[23]<<8 | head[24]<<16 | head[25]<<24;
	size = width*height;
	printf(" Image size : width=%d height=%d\n",width,height);

    //  パレットデータを読み込む（このデータは本プログラムでは使わない）
	// パレットデータ（色テーブル）が入っているbitmapデータでは，ずれてしまうので注意）
//	for(int c=0; c<1024; c++){
//			palette[c] = getc(infile);
//  }

	// 画像メモリ確保
	img = alloc_IMG_YUV(width, height);
	img_out = alloc_IMG_YUV(width, height);
	img_rgb = alloc_IMG_RGB(width, height);
	img_rgb2 = alloc_IMG_RGB(width, height);

	//ビットマップファイルの画像データ部読み込み
	read_bmp(img, img_rgb, infile);

	    // imgにはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //

	setvalue_image(img_out, 128, 128, 128);  //出力画像の初期値にグレー画像をセット

	printf(" Image processing start =====> \n");

//--------------------------処理はここへ--------------------------------------------------

	for(k=0; k<height; k++){
		for(j=0; j<width; j++){

			value = a*img->Y[j + k*width] + b;	  // 輝度をcvalueだけ加算

			img_out->Y[j + k*width] = rounding(value);	  // rounding_integer()は0～255の範囲に収める関数

		}
	}

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------

	printf(" <==== Image processing end \n");

	number = number - (number / 1000) * 1000;
	img_out->Y[0] = rounding_integer(number);
	img_out->Y[1] = rounding(a*10);
	img_out->Y[2] = rounding(b/10);

	//ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	write_bmp(img_out, img_rgb2, outfile);

	//ファイルのクローズとメモリ解放
	fclose(infile);
	fclose(outfile);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}