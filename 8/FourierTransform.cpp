#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"

//------------------------------------------------------------------------------
//	Function-ID	main(FourierTransform.c)
//	Function-Name	
//	Abstract	             �摜�̂Q�����t�[���G�ϊ����s��
//	Argument	nt	argc		= �p�����[�^��
//			char	*argv[]		= �p�����[�^������Q
//	Return-Value	int			= �ԋp�l
//	Special_Desc
//------------------------------------------------------------------------------
int
main(int    argc,   char   *argv[])
{
     IMG_YUV  *img, *imgfp, *img_out;
	 IMG_YUV_DOUBLE  *imgr, *imgi, *imgwr, *imgwi, *imgfr, *imgfi, *imgf_power;
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^

	 FILE *infile, *outfile;
	 char infname1[512];
	 char outfname1[512];

	 int j, k, width, height, size;
	 int number;
	 int head[54];				//�w�b�_�f�[�^�z��

	 uchar array_line[176][16];

     /* �p�����[�^�`�F�b�N */
     if (argc < 4) {
	  fprintf(stderr, "Usage: %s infile outfile number \n", argv[0]);
	  printf("        infile: input file name \n");
	  printf("        outfile: output file name for subsampled image \n");
	  printf("        number: student number \n");
	  exit(-1);
     }

     /* �p�����[�^�ݒ� */
     strcpy(infname1, argv[1]);       /* ���̓t�@�C���� */
     strcpy(outfname1, argv[2]);      /* �o�̓t�@�C���� �i�k���摜�j*/
	 number = atoi(argv[3]);      /* �w���ԍ�(7���j */

     printf(" -- input filename= %s \n", infname1);

	 /* �摜�t�@�C�����I�[�v������ */
	 if (!strcmp(infname1, "-")){
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
	 for (int c = 0; c<54; c++){
		 head[c] = fgetc(infile);
	 }

	 //�w�b�_����̕��ƍ������Z�o
	 width = head[18] | head[19] << 8 | head[20] << 16 | head[21] << 24;
	 height = head[22] | head[23] << 8 | head[24] << 16 | head[25] << 24;
	 size = width*height;
	 printf(" Image size : width=%d height=%d\n", width, height);

	 /* �摜�\���̃������m�� */
     img = alloc_IMG_YUV(width, height);
	 imgfp = alloc_IMG_YUV(width, height);
	 img_out = alloc_IMG_YUV(width, height);
	 imgr = alloc_IMG_YUV_DOUBLE(width, height);
	 imgi = alloc_IMG_YUV_DOUBLE(width, height);
	 imgwr = alloc_IMG_YUV_DOUBLE(width, height);
	 imgwi = alloc_IMG_YUV_DOUBLE(width, height);
	 imgfr = alloc_IMG_YUV_DOUBLE(width, height);
	 imgfi = alloc_IMG_YUV_DOUBLE(width, height);
	 imgf_power = alloc_IMG_YUV_DOUBLE(width, height);

	 img_rgb = alloc_IMG_RGB(width, height);  //��Ɨp�z��
	 img_rgb2 = alloc_IMG_RGB(width, height); //��Ɨp�z��

     /* �摜�����J�n ------------------------*/
	 read_bmp(img, img_rgb, infile);

	   // img�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	   // img_rgb�ɂ�RGB�`���i444�t�H�[�}�b�g�j�̉摜�f�[�^������ //

	 printf(" -- DFT processing start \n");

	 dft_image(img, imgr, imgi, imgwr, imgwi, imgfr, imgfi, imgf_power);   /* DFT */
	 double2uchar_image(imgf_power, imgfp, 1, 1000);      /* DFT�p���[�̐������i1000�{����0-255�ɃN���b�v�j */
	 dft_image_shift(imgfp, img_out);   /* DFT�摜�̃V�t�g(�����������ɂȂ�悤�ɕ��ёւ��j */

	 /* �摜�����I�� ------------------------*/

//	 number = number - (number / 1000) * 1000;
//	 img_out->Y[0] = rounding_integer(number);
	 printf("  === File output: number=%d \n", number);

	 create_number_line(array_line, number);

	 for (k = 0; k<16; k++){
		 for (j = 0; j<176; j++){
			 img_out->Y[j + (k + 4)*width] = array_line[j][k];
			 img_out->U[j + (k + 4)*width] = 128;
			 img_out->V[j + (k + 4)*width] = 128;
		 }
	 }

	 write_bmp(img_out, img_rgb2, outfile);
	 printf(" <==== Image processing end \n");

	fclose(infile);
	fclose(outfile);
	free_IMG_YUV(img);
	free_IMG_YUV(imgfp);
	free_IMG_YUV(img_out);
	free_IMG_YUV_DOUBLE(imgr);
	free_IMG_YUV_DOUBLE(imgi);
	free_IMG_YUV_DOUBLE(imgwr);
	free_IMG_YUV_DOUBLE(imgwi);
	free_IMG_YUV_DOUBLE(imgfr);
	free_IMG_YUV_DOUBLE(imgfi);
	free_IMG_YUV_DOUBLE(imgf_power);
	free_IMG_RGB(img_rgb);
	free_IMG_RGB(img_rgb2);

	return(0);
}


// Copyright (C) CIT-YashimaLab
