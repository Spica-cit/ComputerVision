#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"
int CalcMedian(int array[], int num);

//****************************************************************
// �T���v���v���O�����Ffilter2.cpp
// �]�[�x���t�B���^�����O�ɂ���Đ����G�b�W�����o����
//
//  ���s���@�F filter2.exe inputfilename outputfilename m student_number
//      inputfilename: ���͉摜�t�@�C����(�r�b�g�}�b�v�j
//      inputfilename: �o�͉摜�t�@�C����(�r�b�g�}�b�v�j
//		m: �o�͒l�����{���邩�̒l�idefault=1�j
//		student_number: �w���ԍ�7��
//
//****************************************************************

int main(int argc, char* argv[])
{
     FILE *infile,*outfile;
	 char infname1[512];
     char outfname1[512];
//	 int palette[1024];			//�p���b�g�p�z��
	 int head[54];				//�w�b�_�f�[�^�z��

	 int size;
	 int height,width;
	 int m;
	 int number;

	 IMG_YUV *img, *img_out, *img_work;        //8bit-YUV�f�[�^�̃|�C���^
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^

     /* �p�����[�^�`�F�b�N */
	 if (argc < 5) {    // (�ǂݍ��݃p�����[�^��+1)��菬�����Ă͂����Ȃ�
		 fprintf(stderr, "Usage: %s infname1 outfname1 threshold \n", argv[0]);
		 printf("        infname1: input file name \n");
		 printf("        outfname1: output file name \n");
		 printf("        m: multiplex factor for edge \n");
		 printf("        number: student number \n");
		 exit(-1);
	 }

	 /* �p�����[�^�ݒ� */
	 strcpy(infname1, argv[1]);      /* ���͉摜�t�@�C���� */
     strcpy(outfname1, argv[2]);     /* �o�͉摜�t�@�C���� */
	 m = atoi(argv[3]);      /* �G�b�W�̑傫�������{���邩�����������l */
	 number = atoi(argv[4]);      /* �w���ԍ�(7���j */

	 printf(" -Input parameters --------\n");
	 printf("   -- Input file= %s \n", infname1);
     printf("   -- Output file= %s \n", outfname1);
	 printf("   -- m= %d \n", m);
	 printf("   -- Student number= %d \n", number);

	 /* �摜�t�@�C���̃I�[�v�� */
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

	//�r�b�g�}�b�v�t�@�C���̃w�b�_����ǂݍ���
	for(int c=0; c<54; c++){
		head[c] = fgetc(infile);
	}

	//�w�b�_����̕��ƍ������Z�o
	width  = head[18] | head[19]<<8 | head[20]<<16 | head[21]<<24;
	height = head[22] | head[23]<<8 | head[24]<<16 | head[25]<<24;
	size = width*height;
	printf(" Image size : width=%d height=%d\n",width,height);

//  �p���b�g�f�[�^��ǂݍ��ށi���̃f�[�^�͖{�v���O�����ł͎g��Ȃ��j
//	for(int c=0; c<1024; c++){
//			palette[c] = getc(infile);
//  }

	// �摜�������m��
	 img = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 img_work = alloc_IMG_YUV(width, height);
	 img_rgb = alloc_IMG_RGB(width, height);  //��Ɨp�z��
	 img_rgb2 = alloc_IMG_RGB(width, height); //��Ɨp�z��

	//�r�b�g�}�b�v�t�@�C���̉摜�f�[�^���ǂݍ���
	 read_bmp(img, img_rgb, infile);

	    // img�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	    // img_rgb�ɂ�RGB�`���i444�t�H�[�}�b�g�j�̉摜�f�[�^������ //

	printf(" Image processing start =====> \n");
	setvalue_image(img_out, 0,128,128);     /* �������D�o�͉摜�S�̂̉�f�ɁCY=0, U,V=128�i�����x���j���Z�b�g���� */

//--------------------------�����͂�����--------------------------------------------------

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

	for (k = 1; k < height - 1; k++) {        // �摜�̏㉺�̂ӂ�1��͌v�Z���Ȃ�
		for (j = 1; j < width - 1; j++) {     // �摜�̍��E�̂ӂ�1��͌v�Z���Ȃ�

			/* Sobel filter (horizontal edge detection) */
			value1 = -img->Y[j - 1 + (k - 1) * width] - 2 * img->Y[j + (k - 1) * width] - img->Y[j + 1 + (k - 1) * width]
				+ img->Y[j - 1 + (k + 1) * width] + 2 * img->Y[j + (k + 1) * width] + img->Y[j + 1 + (k + 1) * width];
			value1 = abs(value1);

			value2 = -img->Y[j - 1 + (k - 1) * width] + img->Y[j + 1 + (k - 1) * width]
				- 2 * img->Y[j - 1 + k * width] + 2 * img->Y[j + 1 + k * width]
				- img->Y[j - 1 + (k + 1) * width] + img->Y[j + 1 + (k + 1) * width];
			value2 = abs(value2);

			value = (value1 + value2) * m;  // ���₷���悤��m�{����

			img_out->Y[j + k * width] = rounding_integer(value);  // �����l��0�`255�ɃN���b�v����֐�
		}
	}

//---�����܂ł̏����ŁC�t�@�C���o�͂������摜��img_out�ɓ���Ă�������---
//--------------------------�����͂����܂�------------------------------------------------

	number = number - (number / 1000) * 1000;
	img_out->Y[0] = rounding_integer(number);
	printf("  === File output: number=%d \n", number);

	//�r�b�g�}�b�v�摜�̏�������
	    // img_out�ɏ������ނׂ�YUV�`���̉摜�f�[�^�������Ă��� //
	    // img_rgb2�͏����̂��߂̍�Ɨp������ //
	write_bmp(img_out, img_rgb2, outfile);
	printf(" <==== Image processing end \n");

	//�t�@�C���̃N���[�Y�ƃ��������
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
// �����l���v�Z����֐��FCalcMedian
// �z��array�ɓn���ꂽnum�̐��l�̒����l�����߂�
//
//  ����
//   array[]: ���l���������z��
//   num: ���l�̌�
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
