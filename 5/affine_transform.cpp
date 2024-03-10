#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

int main(int argc, char* argv[])
{

	FILE *fp_in, *fp_out2;
//	FILE *fp_out1;
//	char infname[512], outfname1[512], outfname2[512];
	char infname[512] = "Lenna100(1024x1024).bmp";
	char outfname1[512] = "original.bmp";
	char outfname2[512] = "output.bmp";

	int width=1024, height=1024, size;
	int widthc, heightc, sizec;
	int color_format = 444;
	double a, b, c, d, e, f;
	int jc, kc;
	int student_number, number;
	
//	 int palette[1024];			//パレット用配列
	 int head[54];				//ヘッダデータ配列

	 IMG_YUV *img, *img_out;        //8bit-YUVデータのポインタ
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

	 /* パラメータチェック */
	 if (argc < 8) {
		 fprintf(stderr, "Usage: %s a b c d e f student_number \n", argv[0]);
		 printf("        a:  affine parameter \n");
		 printf("        b:  affine parameter \n");
		 printf("        c:  affine parameter \n");
		 printf("        d:  affine parameter \n");
		 printf("        e:  affine parameter \n");
		 printf("        f:  affine parameter \n");
		 printf("        student_number:  student number of 7 digits \n");
		 exit(-1);
	 }

	 /* パラメータ設定 */
	 a = atof(argv[1]);         /* アフィンパラメータ */
	 b = atof(argv[2]);         /* アフィンパラメータ */
	 c = atof(argv[3]);         /* アフィンパラメータ */
	 d = atof(argv[4]);         /* アフィンパラメータ */
	 e = atof(argv[5]);         /* アフィンパラメータ */
	 f = atof(argv[6]);         /* アフィンパラメータ */
	 student_number = atoi(argv[7]);         /* 7桁の学生番号 */

	 printf(" -- input filename= %s \n", infname);
	 printf(" -- output filename (original)= %s \n", outfname1);
	 printf(" -- output filename (after transform)= %s \n", outfname2);
	 printf(" -- width= %d \n", width);
	 printf(" -- height= %d \n", height);
	 printf(" -- color_format= %d \n", color_format);
	 printf(" -- a= %f \n", a);
	 printf(" -- b= %f \n", b);
	 printf(" -- c= %f \n", c);
	 printf(" -- d= %f \n", d);
	 printf(" -- e= %f \n", e);
	 printf(" -- f= %f \n", f);
	 printf(" -- student_number= %d \n", student_number);

	 size = width * height;

	 if (color_format == 420){
		 widthc = width / 2;
		 heightc = height / 2;
	 }
	 else{
		 widthc = width;
		 heightc = height;
	 }
	 sizec = widthc * heightc;

	 jc = width / 2;
	 kc = height / 2;

	 printf("\n");

	 /* 画像ファイルをオープンする */
	 if (!strcmp(infname, "-")){
		 fp_in = stdin;
	 }
	 else if ((fp_in = fopen(infname, "rb")) == NULL) {
		 fprintf(stderr, "File open error = %s\n", infname);
		 exit(-1);
	 }

//	 sprintf(outfname1, "%s", argv[2]);
//	 if ((fp_out1 = fopen(outfname1, "wb")) == NULL) {
//		 fprintf(stderr, "OutPut File open error = %s\n", outfname1);
//		 exit(-1);
//	 }
//	 sprintf(outfname2, "%s", argv[3]);
	 if ((fp_out2 = fopen(outfname2, "wb")) == NULL) {
		 fprintf(stderr, "OutPut File open error = %s\n", outfname2);
		 exit(-1);
	 }

	//ビットマップファイルのヘッダ部を読み込み
	for(int c=0; c<54; c++){
		head[c] = fgetc(fp_in);
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
	img_rgb = alloc_IMG_RGB(width, height);
	img_rgb2 = alloc_IMG_RGB(width, height);

	//ビットマップファイルの画像データ部読み込み
	read_bmp(img, img_rgb, fp_in);

	    // imgにはYUV形式(444フォーマット）の画像データが入る //
	    // img_rgbにはRGB形式（444フォーマット）の画像データが入る //

	printf(" Image processing start =====> \n");

//--------------------------処理はここへ--------------------------------------------------

	setvalue_image(img_out, 255, 128, 128);

	affine4_image(img, img_out, a, b, c, d, e, f);     /* アフィン変換処理 */

//	draw_axix(img, jc, kc);
	draw_axix(img_out, jc, kc);

//---ここまでの処理で，ファイル出力したい画像をimg_outに入れておくこと---
//--------------------------処理はここまで------------------------------------------------

	number = student_number - (student_number / 1000) * 1000;
	img_out->Y[0] = rounding_integer(number);

	printf(" <==== Image processing end \n");
 
	//ビットマップ画像の書き込み
	    // img_outに書き込むべきYUV形式の画像データが入っている //
	    // img_rgb2は処理のための作業用メモリ //
	/* 原画像出力 */
//	write_bmp(img, img_rgb2, fp_out1);
	/* 処理画像出力 */
	write_bmp(img_out, img_rgb2, fp_out2);   /* 処理画像のファイル出力 */

	//ファイルのクローズとメモリ解放
	fclose(fp_in);
//	fclose(fp_out1);
	fclose(fp_out2);
	free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}