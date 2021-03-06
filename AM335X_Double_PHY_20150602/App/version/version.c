/*****************************************Copyright(C)******************************************
*******************************************浙江方泰*********************************************
*------------------------------------------文件信息---------------------------------------------
* FileName			: version.c
* Author			:
* Date First Issued	: 130722
* Version			: V
* Description		:
*----------------------------------------历史版本信息-------------------------------------------
* History			:
* //2013	        : V
* Description		:
*-----------------------------------------------------------------------------------------------
***********************************************************************************************/
/* Includes-----------------------------------------------------------------------------------*/
#include <stdio.h>
#include "sysconfig.h"
#include "version.h"
#include "def_config.h"


/****************************************************************************************************
**名称:void getVersion(char* str_ver)
**功能:获取版本号:F100.UMB01.U81.1501.00.02
* 入口:无
* 出口:无
**auth:hxj, date: 2015-1-23 17:28
*****************************************************************************************************/
void getVersion(char* str_ver)
{
    volatile char *tmp_ver_info[2] =
    {
        "VER_APP_123456789ABCDEF:",
        APP_VER,
    };
    int ver_len=25;
    char buf[50]={0};

    memcpy(buf,(char *)tmp_ver_info[1],strlen(APP_VER));

    if(NULL !=str_ver)
    {
        memcpy(str_ver,&buf[12],ver_len);
        str_ver[ver_len]=0;
    }

}



/************************(C)COPYRIGHT 2013 浙江方泰*****END OF FILE****************************/


