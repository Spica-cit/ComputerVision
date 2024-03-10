#define _CRT_SECURE_NO_WARNINGS  //���֐���warning�o�͖���

#include "header.h"

//****************************************************************
// �T���v���v���O�����FRecognition.cpp
// �G�b�W���x�ɂ���ē��͉摜���c�Ȃ����Ȃ���F������
//
//  ���s���@�F Recognition.exe 
//      ���̓r�b�g�}�b�v�t�@�C����image1.bmp, image2.bmp�̂悤��
//      1�`20�܂ŘA�ԂŃt�@�C������t����20�������Ă�������
//      
//   �����̔F���A���S���Y���͌�F����������܂�ł���
//****************************************************************

int main(int argc, char* argv[])
{
	FILE *infile, *outfile;

	IMG_YUV *img, *img_out;        //8bit-YUV�f�[�^�̃|�C���^
	IMG_RGB *img_rgb, *img_rgb2;   //8bit-RGB�f�[�^�̃|�C���^

	char infname[512];
	char outfname[512]="out.bmp";
	int head[54];				//�w�b�_�f�[�^�z��

	int size;
	int height, width;
	int j, k;
	int i;
	int js = 300, ks = 200, jsize = 200, ksize = 160;
	int jp[20] = { 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600, 0, 200, 400, 600};
	int kp[20] = { 0, 0, 0, 0, 160, 160, 160, 160, 320, 320, 320, 320, 480, 480, 480, 480, 640, 640, 640, 640 };
	int sw, ms=0;

	/* �����f�[�^�i�ύX���Ȃ����Ɓj */
	int answer[20] = { 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 0 };   /* dataset-1 */

	int student_number;
	uchar array_line[176][16];

	/* �p�����[�^�`�F�b�N */
	if (argc < 2) {
		fprintf(stderr, "Usage: %s student_number \n", argv[0]);
		printf("        student_number: student number(7-digit) \n");
		exit(-1);
	}
	/* �p�����[�^�ݒ� */
	student_number = atoi(argv[1]);      /* �w���ԍ�(7���j */

	/* �o�͉摜�t�@�C���̃I�[�v���ƃ������m�� */
	if ((outfile = fopen(outfname, "wb")) == NULL) {
		fprintf(stderr, "OutPut File open error = %s\n", outfname);
		exit(-1);
	}
	img_out = alloc_IMG_YUV(800, 800);
	img_rgb2 = alloc_IMG_RGB(800, 800); //��Ɨp�z��

	setvalue_image(img_out, 255, 0, 128);  /* yellow */

	/* 20�̉摜�t�@�C�������[�v�J�n --------------*/

	for (i = 1; i <= 20; i++){

		sprintf(infname, "image%d.bmp", i);

		/* �摜�t�@�C���̃I�[�v�� */
		if (!strcmp(infname, "-")){
			infile = stdin;
		}
		else if ((infile = fopen(infname, "rb")) == NULL) {
			fprintf(stderr, "File open error = %s\n", infname);
			exit(-1);
		}


		//�r�b�g�}�b�v�t�@�C���̃w�b�_����ǂݍ���
		for (int c = 0; c < 54; c++){
			head[c] = fgetc(infile);
		}

		//�w�b�_����̕��ƍ������Z�o
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

		// �摜�������m��
		img = alloc_IMG_YUV(width, height);
		img_rgb = alloc_IMG_RGB(width, height);  //��Ɨp�z��

		//�r�b�g�}�b�v�t�@�C���̉摜�f�[�^���ǂݍ��݁iimg�ɂ�YUV�`��(444�t�H�[�}�b�g�j�̓��͉摜�f�[�^������j
		read_bmp(img, img_rgb, infile);

		//----**** ���菈���͂������� ****---------------------------------------

		int sum = 0;
		int value;

		for (k = 1; k < height - 1; k++){
			for (j = 1; j < width - 1; j++){

				/* Sobel filter (horizontal edge detection) �����G�b�W���o�]�[�x���t�B���^ */
				value = -img->Y[j - 1 + (k - 1)*width] - 2 * img->Y[j + (k - 1)*width] - img->Y[j + 1 + (k - 1)*width]
					+ img->Y[j - 1 + (k + 1)*width] + 2 * img->Y[j + (k + 1)*width] + img->Y[j + 1 + (k + 1)*width];
				value = abs(value);
				sum = sum + value;

			}
		}

		printf("     sum = %d \n", sum);

		int th = 20000000;
		if ((5000000 < sum && sum < 7000000) || (20000000 < sum && sum < 25000000) || (40000000 < sum && sum < 51000000) || 60000000 < sum){   /* �����G�b�W�o�͂̍��v��th���傫����Ή��� */
			sw = 0;
		}
		else{     /* ����ȊO�͏c�� */
			sw = 1;
		}

		//---�����܂ł̏����ŁC���ȉ摜�Ȃ�sw=0�C�c�ȉ摜�Ȃ�sw=1�����Ă���---

		//-----**** ���菈���͂����܂� ****------------------------------

		if (sw == 0){
			printf("     Recognition result: Horizontal \n");
		}
		else{
			printf("     Recognition result: Vertical \n");
		}

		if ((sw + answer[i - 1]) == 1){
			ms++;   /* �딻��̐� */
			draw_rectangle(img_out, jp[i - 1], kp[i - 1], jsize, ksize);
		}
		else{
			cut_and_paste(img_out, img, img_out, js, ks, jsize, ksize, jp[i - 1], kp[i - 1]);
		}

		//�t�@�C���̃N���[�Y�ƃ��������
		fclose(infile);
		free_IMG_YUV(img);
		free_IMG_RGB(img_rgb);

	}
	/* 20�̉摜�t�@�C���̃��[�v�I�� --------------*/

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

	//�摜�t�@�C���o��
	write_bmp(img_out, img_rgb2, outfile);
	
	//�t�@�C���̃N���[�Y�ƃ��������
	fclose(outfile);
	free_IMG_YUV(img_out);
	free_IMG_RGB(img_rgb2);

	return 0;
}