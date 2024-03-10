#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

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
	
//	 int palette[1024];			//�p���b�g�p�z��
	 int head[54];				//�w�b�_�f�[�^�z��

	 IMG_YUV *img, *img_out;        //8bit-YUV�f�[�^�̃|�C���^
	 IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^

	 /* �p�����[�^�`�F�b�N */
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

	 /* �p�����[�^�ݒ� */
	 a = atof(argv[1]);         /* �A�t�B���p�����[�^ */
	 b = atof(argv[2]);         /* �A�t�B���p�����[�^ */
	 c = atof(argv[3]);         /* �A�t�B���p�����[�^ */
	 d = atof(argv[4]);         /* �A�t�B���p�����[�^ */
	 e = atof(argv[5]);         /* �A�t�B���p�����[�^ */
	 f = atof(argv[6]);         /* �A�t�B���p�����[�^ */
	 student_number = atoi(argv[7]);         /* 7���̊w���ԍ� */

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

	 /* �摜�t�@�C�����I�[�v������ */
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

	//�r�b�g�}�b�v�t�@�C���̃w�b�_����ǂݍ���
	for(int c=0; c<54; c++){
		head[c] = fgetc(fp_in);
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
	img_rgb = alloc_IMG_RGB(width, height);
	img_rgb2 = alloc_IMG_RGB(width, height);

	//�r�b�g�}�b�v�t�@�C���̉摜�f�[�^���ǂݍ���
	read_bmp(img, img_rgb, fp_in);

	    // img�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̉摜�f�[�^������ //
	    // img_rgb�ɂ�RGB�`���i444�t�H�[�}�b�g�j�̉摜�f�[�^������ //

	printf(" Image processing start =====> \n");

//--------------------------�����͂�����--------------------------------------------------

	setvalue_image(img_out, 255, 128, 128);

	affine4_image(img, img_out, a, b, c, d, e, f);     /* �A�t�B���ϊ����� */

//	draw_axix(img, jc, kc);
	draw_axix(img_out, jc, kc);

//---�����܂ł̏����ŁC�t�@�C���o�͂������摜��img_out�ɓ���Ă�������---
//--------------------------�����͂����܂�------------------------------------------------

	number = student_number - (student_number / 1000) * 1000;
	img_out->Y[0] = rounding_integer(number);

	printf(" <==== Image processing end \n");
 
	//�r�b�g�}�b�v�摜�̏�������
	    // img_out�ɏ������ނׂ�YUV�`���̉摜�f�[�^�������Ă��� //
	    // img_rgb2�͏����̂��߂̍�Ɨp������ //
	/* ���摜�o�� */
//	write_bmp(img, img_rgb2, fp_out1);
	/* �����摜�o�� */
	write_bmp(img_out, img_rgb2, fp_out2);   /* �����摜�̃t�@�C���o�� */

	//�t�@�C���̃N���[�Y�ƃ��������
	fclose(fp_in);
//	fclose(fp_out1);
	fclose(fp_out2);
	free_IMG_YUV(img);
    free_IMG_YUV(img_out);
    free_IMG_RGB(img_rgb);
    free_IMG_RGB(img_rgb2);

	return 0;
}