#define _CRT_SECURE_NO_WARNINGS  //旧関数のwarning出力無視

#include "header.h"

//****************************************************************
// サンプルプログラム：Recognition.cpp
// エッジ強度によって入力画像が縦縞が横縞かを認識する
//
//  実行方法： Recognition.exe 
//      入力ビットマップファイルはimage1.bmp, image2.bmpのように
//      1～20まで連番でファイル名を付けて20個準備しておくこと
//      
//   ★この認識アルゴリズムは誤認識する問題を含んでいる
//****************************************************************

int main(int argc, char* argv[])
{
	FILE *infile, *outfile;

	IMG_YUV *img, *img_out;        //8bit-YUVデータのポインタ
	IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGBデータのポインタ

	char infname[512];
	char outfname[512]="out.bmp";
	int head[54];				//ヘッダデータ配列

	int size;
	int height, width;
	int j, k;
	int i;
	int js = 300, ks = 200, jsize = 200, ksize = 160;
	int jp[20] = { 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600};
	int kp[20] = { 0, 0, 0, 0, 160, 160, 160, 160, 320, 320, 320, 320, 480, 480, 480, 480, 640, 640, 640, 640 };
	int sw, ms=0;

	/* 正解データ（変更しないこと） */
	int answer[20] = { 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 };   /* dataset-1 */

	int student_number;
	uchar array_line[176][16];

	/* パラメータチェック */
	if (argc < 2) {
		fprintf(stderr, "Usage: %s student_number \n", argv[0]);
		printf("        student_number: student number(7-digit) \n");
		exit(-1);
	}
	/* パラメータ設定 */
	student_number = atoi(argv[1]);      /* 学生番号(7桁） */

	/* 出力画像ファイルのオープンとメモリ確保 */
	if ((outfile = fopen(outfname, "wb")) == NULL) {
		fprintf(stderr, "OutPut File open error = %s\n", outfname);
		exit(-1);
	}
	img_out = alloc_IMG_YUV(800, 800);
	img_rgb2 = alloc_IMG_RGB(800, 800); //作業用配列

	setvalue_image(img_out, 255, 0, 128);  /* yellow */

	/* 20個の画像ファイルをループ開始 --------------*/

	for (i = 1; i <= 20; i++){

		sprintf(infname, "image%d.bmp", i);

		/* 画像ファイルのオープン */
		if (!strcmp(infname, "-")){
			infile = stdin;
		}
		else if ((infile = fopen(infname, "rb")) == NULL) {
			fprintf(stderr, "File open error = %s\n", infname);
			exit(-1);
		}


		//ビットマップファイルのヘッダ部を読み込み
		for (int c = 0; c < 54; c++){
			head[c] = fgetc(infile);
		}

		//ヘッダからの幅と高さを算出
		width = head[18] | head[19] << 8 | head[20] << 16 | head[21] << 24;
		height = head[22] | head[23] << 8 | head[24] << 16 | head[25] << 24;
		size = width*height;
		printf("\n");
		printf("-- Input file= %s : width=%d, height=%d, ", infname, width, height);
		if (answer[i-1] == 0){
			printf("true answer= Horizontal\n");
		}
		else{
			printf("true answer= Vertical\n");
		}

		// 画像メモリ確保
		img = alloc_IMG_YUV(width, height);
		img_rgb = alloc_IMG_RGB(width, height);  //作業用配列

		//ビットマップファイルの画像データ部読み込み（imgにはYUV形式(444フォーマット）の入力画像データが入る）
		read_bmp(img, img_rgb, infile);

		//----**** 判定処理はここから ****---------------------------------------

		int sum = 0;
		int value;

		for (k = 1; k < height - 1; k++){
			for (j = 1; j < width - 1; j++){

				/* Sobel filter (horizontal edge detection) 水平エッジ検出ゾーベルフィルタ */
				value = -img->Y[j - 1 + (k - 1)*width] - 2 * img->Y[j + (k - 1)*width] - img->Y[j + 1 + (k - 1)*width]
					+ img->Y[j - 1 + (k + 1)*width] + 2 * img->Y[j + (k + 1)*width] + img->Y[j + 1 + (k + 1)*width];
				value = abs(value);
				sum = sum + value;

			}
		}

		printf("     sum = %d \n", sum);

		int th = 20000000;
		if ((5000000 < sum && sum < 7000000) || (20000000 < sum && sum < 25000000) || (40000000 < sum && sum < 51000000) || 60000000 < sum){   /* 水平エッジ出力の合計がthより大きければ横縞 */
			sw = 0;
		}
		else{     /* それ以外は縦縞 */
			sw = 1;
		}

		//---ここまでの処理で，横縞画像ならsw=0，縦縞画像ならsw=1を入れておく---

		//-----**** 判定処理はここまで ****------------------------------

		if (sw == 0){
			printf("     Recognition result: Horizontal \n");
		}
		else{
			printf("     Recognition result: Vertical \n");
		}

		if ((sw + answer[i - 1]) == 1){
			ms++;   /* 誤判定の数 */
			draw_rectangle(img_out, jp[i - 1], kp[i - 1], jsize, ksize);
		}
		else{
			cut_and_paste(img_out, img, img_out, js, ks, jsize, ksize, jp[i - 1], kp[i - 1]);
		}

		//ファイルのクローズとメモリ解放
		fclose(infile);
		free_IMG_YUV(img);
		free_IMG_RGB(img_rgb);

	}
	/* 20個の画像ファイルのループ終了 --------------*/

	printf("\n");
	printf("=== Total performance ========================= \n");
	printf("    Correct:%d, Miss:%d, Total Recognition Accuracy = %6.2f \n", 20 - ms, ms, ((20 - (double)ms) * 100) / 20);

	create_number_line(array_line, student_number);

	for (k = 0; k<16; k++){
		for (j = 0; j<176; j++){
			img_out->Y[j + (k + 4)*width] = array_line[j][k];
			img_out->U[j + (k + 4)*width] = 128;
			img_out->V[j + (k + 4)*width] = 128;
		}
	}

	//画像ファイル出力
	write_bmp(img_out, img_rgb2, outfile);
	
	//ファイルのクローズとメモリ解放
	fclose(outfile);
	free_IMG_YUV(img_out);
	free_IMG_RGB(img_rgb2);

	return 0;
}