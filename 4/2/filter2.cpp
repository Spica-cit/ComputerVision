#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"
int CalcMedian(int array[], int num);

//****************************************************************
// サンプルプログラム：filter2.cpp
// ゾーベルフィルタリングによって水平エッジを検出する
//
//  実行方法： filter2.exe inputfilename outputfilename m student_number
//      inputfilename: 入力画像ファイル名(ビットマップ）
//      inputfilename: 出力画像ファイル名(ビットマップ）
//		m: 出力値を何倍するかの値（default=1）
//		student_number: 学生番号7桁
//
//****************************************************************

int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 int size;
	 int height,width;
	 int m;
	 int number;

	 IMG_YUV *img, *img_out, *img_work;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

     /* パラメータチェック */
	 if (argc < 5) {    // (読み込みパラメータ数+1)より小さくてはいけない
		 fprintf(stderr, "Usage: %s infname1 outfname1 threshold \n", argv[0]);
		 printf("        infname1: input file name \n");
		 printf("        outfname1: output file name \n");
		 printf("        m: multiplex factor for edge \n");
		 printf("        number: student number \n");
		 exit(-1);
	 }

	 /* パラメータ設定 */
	 strcpy(infname1, argv[1]);      /* 入力画像ファイル名 */
     strcpy(outfname1, argv[2]);     /* 出力画像ファイル名 */
	 m = atoi(argv[3]);      /* エッジの大きさを何倍するかを示す整数値 */
	 number = atoi(argv[4]);      /* 学生番号(7桁） */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
	 printf("   -- m= %d \n", m);
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
//	for(int c=0; c<1024; c++){
//			palette[c] = getc(infile);
//  }

	// 画像メモリ確保
	 img = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 img_work = alloc_IMG_YUV(width, height);
	 img_rgb = alloc_IMG_RGB(width, height);  //作業用配列
	 img_rgb2 = alloc_IMG_RGB(width, height); //作業用配列

	//ビットマップファイルの画像データ部読み込み
	 read_bmp(img, img_rgb, infile);

	    // imgにはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //

	printf(" Image processing start =====> \n");
	setvalue_image(img_out, 0,128,128);     /* 初期化．出力画像全体の画素に，Y=0, U,V=128（黒レベル）をセットする */

//--------------------------処理はここへ--------------------------------------------------

	int j, k;
	int value, value1, value2;
	int median[9];

	for (k = 1; k < height - 1; k++) {
		for (j = 1; j < width - 1; j++) {
			median[1] = img->Y[j - 1 + (k - 1) * width];
			median[2] = img->Y[j - 1 + k * width];
			median[3] = img->Y[j - 1 + (k + 1) * width];
			median[4] = img->Y[j + (k - 1) * width];
			median[5] = img->Y[j + k * width];
			median[6] = img->Y[j + (k + 1) * width];
			median[7] = img->Y[j + 1 + (k - 1) * width];
			median[8] = img->Y[j + 1 + k * width];
			median[9] = img->Y[j + 1 + (k + 1) * width];

			img->Y[j + k * width] = CalcMedian(median, 9);
		}
	}

	for (k = 1; k < height - 1; k++) {        // 画像の上下のふち1列は計算しない
		for (j = 1; j < width - 1; j++) {     // 画像の左右のふち1列は計算しない

			/* Sobel filter (horizontal edge detection) */
			value1 = -img->Y[j - 1 + (k - 1) * width] - 2 * img->Y[j + (k - 1) * width] - img->Y[j + 1 + (k - 1) * width]
				+ img->Y[j - 1 + (k + 1) * width] + 2 * img->Y[j + (k + 1) * width] + img->Y[j + 1 + (k + 1) * width];
			value1 = abs(value1);

			value2 = -img->Y[j - 1 + (k - 1) * width] + img->Y[j + 1 + (k - 1) * width]
				- 2 * img->Y[j - 1 + k * width] + 2 * img->Y[j + 1 + k * width]
				- img->Y[j - 1 + (k + 1) * width] + img->Y[j + 1 + (k + 1) * width];
			value2 = abs(value2);

			value = (value1 + value2) * m;  // 見やすいようにm倍する

			img_out->Y[j + k * width] = rounding_integer(value);  // 整数値を0～255にクリップする関数
		}
	}

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------

	number = number - (number / 1000) * 1000;
	img_out->Y[0] = rounding_integer(number);
	printf("  === File output: number=%d \n", number);

	//ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	write_bmp(img_out, img_rgb2, outfile);
	printf(" <==== Image processing end \n");

	//ファイルのクローズとメモリ解放
	fclose(infile);
	fclose(outfile);
    free_IMG_YUV(img);
    free_IMG_YUV(img_out);
	free_IMG_YUV(img_work);
	free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}

//****************************************************************
// 中央値を計算する関数：CalcMedian
// 配列arrayに渡されたnum個の数値の中央値を求める
//
//  引数
//   array[]: 数値が入った配列
//   num: 数値の個数
//
//****************************************************************
int CalcMedian(int array[], int num)
{
	int pt, median;
	int i, v;

	pt = num;
	for (i = 0; i<num; i++){
		for (v = 0; v<pt - 1; v++){
			if (array[v] < array[v + 1]){
				int tmp = array[v + 1];
				array[v + 1] = array[v];
				array[v] = tmp;
			}
		}
		pt = pt - 1;
	}

	median = array[num / 2 + 1];
	return  median;
}
