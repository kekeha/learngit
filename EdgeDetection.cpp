void vDepthEdg(sEDG *psDepthEdg)
{
	USHORT unWidth, unHeight;
	USHORT unData3x3[9];
	int i, j;
	USHORT *unImageBuff;
	USHORT unAbsSblH, unAbsSblV, unDepthEdgVlu;
	USHORT unDepthLocVlu, unDepthEdgTh;
	USHORT unDepthAve[4];
	USHORT unDepthLocVluTh[3];
	LONG lTmp;
	
	unWidth = psDepthEdg->unH;
	unHeight = psDepthEdg->unV;


	unImageBuff = (USHORT *)calloc(sizeof(USHORT), unWidth*unHeight);

	
	for (j = 0; j < unHeight; j++) {
		for (i = 0; i < unWidth; i++) {
			unImageBuff[i + j * unWidth] = (psDepthEdg->punDepth[i + j * unWidth] >> 4);
		}
	}

	for (j = 1; j < (unHeight - 1); j++) {
		for (i = 1; i < (unWidth - 1); i++) {

			
			if (psDepthEdg->punDepth[i + j * unWidth] != 0xFFFF &&
				psDepthEdg->punDepth[i - 1 + (j - 1)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + (j - 1)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + 1 + (j - 1)*unWidth] != 0 &&
				psDepthEdg->punDepth[i - 1 + (j + 0)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + (j + 0)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + 1 + (j + 0)*unWidth] != 0 &&
				psDepthEdg->punDepth[i - 1 + (j + 1)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + (j + 1)*unWidth] != 0 &&
				psDepthEdg->punDepth[i + 1 + (j + 1)*unWidth] != 0)
			{
				/*
					0 1 2
					3 4 5
					6 7 8	*/
				unData3x3[0] = unImageBuff[i - 1 + (j - 1)*unWidth];
				unData3x3[1] = unImageBuff[i + (j - 1)*unWidth];
				unData3x3[2] = unImageBuff[i + 1 + (j - 1)*unWidth];
				unData3x3[3] = unImageBuff[i - 1 + (j + 0)*unWidth];
				unData3x3[4] = unImageBuff[i + (j + 0)*unWidth];
				unData3x3[5] = unImageBuff[i + 1 + (j + 0)*unWidth];
				unData3x3[6] = unImageBuff[i - 1 + (j + 1)*unWidth];
				unData3x3[7] = unImageBuff[i + (j + 1)*unWidth];
				unData3x3[8] = unImageBuff[i + 1 + (j + 1)*unWidth];

				//�����\�[�x���t�B���^(����15's)
				lTmp = (-1) * unData3x3[0] + (1) * unData3x3[2] +
					(-2) * unData3x3[3] + (2) * unData3x3[5] +
					(-1) * unData3x3[6] + (1) * unData3x3[8];
				unAbsSblH = (USHORT)abs(lTmp);
				//12bit clip
				unAbsSblH = ((unAbsSblH > 0xFFF) ? 0xFFF : unAbsSblH);

				//�����\�[�x���t�B���^(����15's)
				lTmp = (-1) * unData3x3[0] + (-2) * unData3x3[1] + (-1) * unData3x3[2] +
					(1) * unData3x3[6] + (2) * unData3x3[7] + (1) * unData3x3[8];
				unAbsSblV = (USHORT)abs(lTmp);
				//12bit clip
				unAbsSblV = ((unAbsSblV > 0xFFF) ? 0xFFF : unAbsSblV);

				//�G�b�W�ő�l
				unDepthEdgVlu = ((unAbsSblH > unAbsSblV) ? unAbsSblH : unAbsSblV);


				//�����̋Ǐ����ϒl��Z�o
				unDepthAve[0] = (unData3x3[0] + unData3x3[1]) / 2;	// a
				unDepthAve[1] = (unData3x3[3] + unData3x3[6]) / 2;	// b
				unDepthAve[2] = (unData3x3[7] + unData3x3[8]) / 2;	// c
				unDepthAve[3] = (unData3x3[2] + unData3x3[5]) / 2;	// d

				//���ёւ���s���A�łQ�Ԗڂɏ����������f�[�^��o�͂�����
				qsort((void *)unDepthAve, 4, sizeof(unDepthAve[0]), USHORT_sort);
				unDepthLocVlu = unDepthAve[1];

				//Depth�G�b�W臒l��ݒ�?
				unDepthLocVluTh[0] = psDepthEdg->unDepthVluX[0];
				unDepthLocVluTh[1] = psDepthEdg->unDepthVluX[0] + (1 << psDepthEdg->unDepthVluX[1]);
				unDepthLocVluTh[2] = unDepthLocVluTh[1] + (1 << psDepthEdg->unDepthVluX[2]);

				if (unDepthLocVlu < unDepthLocVluTh[0]) {
					unDepthEdgTh = psDepthEdg->unDepthEdgY[0];
 
				}
				else if (unDepthLocVlu < unDepthLocVluTh[1]) {
					lTmp = (LONG)(((unDepthLocVlu - unDepthLocVluTh[0]) * (psDepthEdg->unDepthEdgY[1] - psDepthEdg->unDepthEdgY[0])) >> psDepthEdg->unDepthVluX[1]);
					unDepthEdgTh = (USHORT)(lTmp + psDepthEdg->unDepthEdgY[0]);

				}
				else if (unDepthLocVlu < unDepthLocVluTh[2]) {
					lTmp = (LONG)(((unDepthLocVlu - unDepthLocVluTh[1]) * (psDepthEdg->unDepthEdgY[2] - psDepthEdg->unDepthEdgY[1])) >> psDepthEdg->unDepthVluX[2]);
					unDepthEdgTh = (USHORT)(lTmp + psDepthEdg->unDepthEdgY[1]);

				}
				else {
					unDepthEdgTh = psDepthEdg->unDepthEdgY[2];
				}

				//�G�b�W臒l����
				if (unDepthEdgVlu > unDepthEdgTh) {
					// �������ُ�l�Ƃ���
					psDepthEdg->punDepth[i + j * unWidth] = 0xFFFF;
				}

			}

		}// End for(i)
	}// End for(j)

	free(unImageBuff);
}

