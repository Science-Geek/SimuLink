/***************************************************************
* Copyright (C) 2011, Wuhan University
* All rights reserved.
*
* 文件名：	ImgVVD_Init.c
* 功能描述: 图像DCT面额版本方向识别初始化函数
*
* 当前版本：1.0
* 作    者：张维夏
* 完成日期：2016年9月20日
*
***************************************************************/


//#include <stdafx.h>


#include   <stdio.h>
#include "ForCur.h"
#include <string.h>


char currencyTypeName[32][8] = {
	"CNY", "TWD", "HKD", "USD", "EURO", "JPY", "GBP", "AUD", "CAD", "SGD",
	"CHF", "RUB", "MOP", "KRW", "PHP", "ZAR", "ETB", "SAR", "PGK", "THB",
	"NZD", "DKK", "NOK", "SEK", "IDR", "BUK", "VND", "AED", "TRY", "MYR", " ", " "
};


char DCTpara[5120];
char TDCT4para[77824];


int DCTParInit()
{
	long i = 0;

	gParConfigUnion.VerInfo.D500000_Num = 0;
	gParConfigUnion.VerInfo.D200000_Num = 0;
	gParConfigUnion.VerInfo.D100000_Num = 0;
	gParConfigUnion.VerInfo.D50000_Num = 0;
	gParConfigUnion.VerInfo.D20000_Num = 0;
	gParConfigUnion.VerInfo.D10000_Num = 0;
	gParConfigUnion.VerInfo.D5000_Num = 0;
	gParConfigUnion.VerInfo.D2000_Num = 0;
	gParConfigUnion.VerInfo.D1000_Num = 0;
	gParConfigUnion.VerInfo.D500_Num = 0;
	gParConfigUnion.VerInfo.D200_Num = 0;
	gParConfigUnion.VerInfo.D100_Num = 0;
	gParConfigUnion.VerInfo.D50_Num = 0;
	gParConfigUnion.VerInfo.D20_Num = 0;
	gParConfigUnion.VerInfo.D10_Num = 0;
	gParConfigUnion.VerInfo.D5_Num = 0;
	gParConfigUnion.VerInfo.D2_Num = 0;
	gParConfigUnion.VerInfo.D1_Num = 0;
	gParConfigUnion.pParConfigUnionSave = (ParConfigUnionSave_*)&gParConfigUnionSave;

	gParConfigUnion.RunningMode = DETECT;

	FILE *templateFile = fopen(TEMPLATE_PATH, "rb");
	if (templateFile == NULL)
	{
		return RET_ERROR;
	}
	fread((unsigned char *)&ParConfigCurrency, 1, sizeof(ParConfigCurrency), templateFile);

	//FlashLoadFlash(0x90200000,sizeof(ParConfigCurrency),(unsigned char *)&ParConfigCurrency);
	if (ParConfigCurrency.magicNum != FOR_CUR_TEMP_CONFIG_MAGIC)
	{
		return RET_ERROR;
	}
	ParConfigCurrency.pDCT = (unsigned char*)DCTpara;
	if (ParConfigCurrency.DCT_len > sizeof(DCTpara))
		return RET_ERROR;
	fseek(templateFile, ParConfigCurrency.DCTStartOffset, 0);
	fread(ParConfigCurrency.pDCT, 1, ParConfigCurrency.DCT_len, templateFile);
	//FlashLoadFlash(0x90200000+ParConfigCurrency.DCTStartOffset,ParConfigCurrency.DCT_len,ParConfigCurrency.pDCT);

	//	ParConfigCurrency.pTDCT3CH = TDCT3para;
	//	if(ParConfigCurrency.TDCT3CH_len > sizeof(TDCT3para))
	//		return RET_ERROR;
	//	FlashLoadFlash(0x90200000+ParConfigCurrency.TDCT3chStartOffset,ParConfigCurrency.TDCT3CH_len,ParConfigCurrency.pTDCT3CH);

	ParConfigCurrency.pTDCT = (unsigned char*)TDCT4para;
	if (ParConfigCurrency.TDCT_len > sizeof(TDCT4para))
		return RET_ERROR;
	//FlashLoadFlash(0x90200000+ParConfigCurrency.TDCTStartOffset,ParConfigCurrency.TDCT_len,ParConfigCurrency.pTDCT);
	fseek(templateFile, ParConfigCurrency.TDCTStartOffset, 0);
	fread(ParConfigCurrency.pTDCT, 1, ParConfigCurrency.TDCT_len, templateFile);
	fclose(templateFile);

	memcpy((void*)&gParConfigUnion.ProgVerInfo, (void*)&ParConfigCurrency.ProgVerInfo, sizeof(ProgVerInfo_));
	return RET_OK;
}


int TemplateSet(int currency)
{
	char curTypeName[CURR_NAME_LEN];
	short i, j, cnt;
	unsigned long ExclThrd = 0;
	strcpy(curTypeName, currencyTypeName[currency]);
	if (Str2CurrType(curTypeName, &(gParConfigUnion.curType)) == RET_ERROR)
	{
		gParConfigUnion.RunningMode = DISABLE;
		return RET_ERROR;
	}
	else
	{
		if (ParConfigCurrency.magicNum == FOR_CUR_TEMP_CONFIG_MAGIC)
		{
			for (i = 0; i < MAX_CURRENCY_NUM; i++)
			{
				if (ParConfigCurrency.ParConfigUnionSave[i] != 0)
				{

					FILE *templateFile = fopen(TEMPLATE_PATH, "rb");
					if (templateFile == NULL)
					{
						return RET_ERROR;
					}
					fseek(templateFile, (unsigned long)ParConfigCurrency.ParConfigUnionSave[i] - 0x90200000, 0);
					fread((unsigned char *)&gParConfigUnionSave, 1, sizeof(gParConfigUnionSave), templateFile);
					fclose(templateFile);

					if (strcmp(gParConfigUnionSave.CurrencyName, currencyTypeName[currency]) == 0)  //遍历各币种直至找到指定币种(currency)
					{
						cnt = 0;
						for (j = 0; j < MAX_CONFIG_NUM; j++)
						{
							if (ParConfigCurrency.pH_Template[i][j] != 0)
							{
								//								ExclThrd+=gParConfigUnionSave.TemplateInfo[j].NP_Sub;

								FILE *templateFile = fopen(TEMPLATE_PATH, "rb");
								if (templateFile == NULL)
								{
									return RET_ERROR;
								}
								fseek(templateFile, (unsigned long)(ParConfigCurrency.pH_Template[i][j]) - 0x90200000, 0);
								fread((unsigned char *)&gExincludeTemplate[j * 4], 1, sizeof(DCTHanMingCode) * 4, templateFile);
								fclose(templateFile);

								cnt++;
							}
						}
						if (cnt == 0)
						{
							gParConfigUnion.RunningMode = DISABLE;
							return RET_ERROR;
						}
						gParConfigUnionSave.ValidTemplateNum = cnt;
						if (gParConfigUnionSave.templteType == 0)
							gParConfigUnion.RunningMode = DETECT;
						return RET_OK;
					}
				}
			}

		}
	}
	//gParConfigUnion.RunningMode = DISABLE;
	return RET_ERROR;

}

