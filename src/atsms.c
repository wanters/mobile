/**************************************************************************
*
* Copyright (c) 2017, luotang.me <wypx520@gmail.com>, China.
* All rights reserved.
*
* Distributed under the terms of the GNU General Public License v2.
*
* This software is provided 'as is' with no explicit or implied warranties
* in respect of its properties, including, but not limited to, correctness
* and/or fitness for purpose.
*
**************************************************************************/

#include "atmod.h"
#include "atsms.h"
#include "atchannel.h"
#include <unistd.h>
#include <stdlib.h>
#include "at_tok.h"
#include "SMS.h"
#include "utf.h"

#define DSC_to_msg(DSC) (DSC == 0 ? "Bit7" : (DSC == 1 ? "Bit8" : "UCS2"))

static int sms_init(void *data, unsigned int datalen);
static int sms_deinit(void *data, unsigned int datalen);
static int sms_start(void *data, unsigned int datalen);
static int sms_stop(void *data, unsigned int datalen);
static int sms_set_info(void *data, unsigned int datalen);
static int sms_get_info(void *data, unsigned int datalen);
static int sms_test(void *data, unsigned int datalen);
static int sms_send_status(void *data, unsigned int datalen);

struct sms_info  smsinfo;
struct sms_info *sms_t = &smsinfo;


struct usbev_module usbdev_sms =
{
    ._init 		= sms_init,
    ._deinit 	= sms_deinit,
    ._start 	= sms_start,
    ._stop 		= sms_stop,
    ._set_info 	= sms_set_info,
    ._get_info 	= sms_get_info,
};

static char *get_number(char *number_string, char *number)
{
    int i;
    for (i = 0; i < NUMBER_LENGTH; i++)
    {
        sprintf(number_string + 4 * i, "00%02X", number[i]);
    }
    number_string[NUMBER_LENGTH * 4] = 0;

    //printf("number_string: %s\n", number_string);
    return number_string;
}

void sms_receive_free(struct SMS_Struct sms_packet)
{
	if (sms_packet.SCA != NULL)
	{
		free(sms_packet.SCA);
		sms_packet.SCA = NULL;
	}

	if (sms_packet.OA != NULL)
	{
		free(sms_packet.OA);
		sms_packet.OA = NULL;
	}	

	if (sms_packet.SCTS != NULL)
	{
		free(sms_packet.SCTS);
		sms_packet.SCTS = NULL;
	}		

	if (sms_packet.UDH != NULL)
	{
		if (sms_packet.UDH->UDH != NULL)
		{
			if (sms_packet.UDH->UDH->IED != NULL)
			{
				free(sms_packet.UDH->UDH->IED);
				sms_packet.UDH->UDH->IED = NULL;
			}

			free(sms_packet.UDH->UDH);
			sms_packet.UDH->UDH = NULL;
		}
		free(sms_packet.UDH);
		sms_packet.UDH = NULL;
	}	

	if (sms_packet.UD != NULL)
	{
		free(sms_packet.UD);
		sms_packet.UD = NULL;
	}	

}

void sms_send_free(struct PDUS *pdus)
{
	unsigned int i;
	for (i = 0; i < pdus->count; i++)
    {
        printf("第 %d 条:\n", i + 1);
        if (pdus->PDU[i] != NULL)
        {
			free(pdus->PDU[i]);
			pdus->PDU[i] = NULL;
        }
    }
}

void sms_receive_printf(const char *sms_messsge)
{
    struct SMS_Struct s = PDUDecoding(sms_messsge);

    printf("服务中心地址: %s\n", s.SCA);
    printf("发送方地址: %s\n", s.OA);
    printf("服务中心时间戳: %s\n", s.SCTS);
    printf("用户数据头: %d\n", s.UDHI);
    printf("消息内容: %s\n", s.UD);
    printf("数据编码方案： %s\n", DSC_to_msg(s.DCS));
    printf("消息类型： %d\n", s.MC);	
    sms_receive_free(s);
}
/*	pdu短信：
	SCA：短信中心服务号码
	DA：目的号码
	UDC：用户数据
	UDHS：用户数据头
*/
int sms_send_printf(char *SCA, char *DA, char *UDC, struct UDHS *udhs)
{
	unsigned int i;
    ATResponse *p_response = NULL;
    int err = -1;
    int ret = -1;
    char *smsLine = NULL;
    char *smsResult = NULL;
    char smsCmd[100] = {0};
    int smsPosition;

    //struct PDUS *pdus = PDUEncoding("+8613080005005","+8613811420981", msg, NULL);
    struct PDUS *pdus = PDUEncoding("", DA, UDC, udhs);
    printf("服务中心地址: %s\n", SCA);
    printf("接收方地址: %s\n", DA);
    printf("消息内容: %s\n", UDC);

    for (i = 0; i < pdus->count; i++)
    {
        printf("第 %d 条:\n", i + 1);
        printf("%s\n", pdus->PDU[i]);
#if 0
	    sprintf(smsCmd, "AT+CMGS=\"%d\"", strlen(pdus->PDU[i])-(strlen(SCA));
	    err = at_send_command_sms(smsCmd, pdus->PDU[i], "+CMGS:", &p_response);
	    if (err != 0)
	    {
	        ret = SMS_ERROR;
	        goto done;
	    }

	    switch (at_get_cme_error(p_response))
	    {
	    case CME_SUCCESS:
	        break;

	    default:
	        ret = SMS_ERROR;
	        goto done;
	    }

	    /* CMGS has succeeded, now look at the result */

	    smsLine = p_response->p_intermediates->line;
	    err = at_tok_start(&smsLine);

	    if (err < 0)
	    {
	        ret = SMS_ERROR;
	        goto done;
	    }

	    err = at_tok_nextint(&smsLine, &smsPosition);

	    if (err < 0)
	    {
	        ret = SMS_ERROR;
	        goto done;
	    }

	    at_response_free(p_response);
	    p_response = NULL;
	    smsResult = NULL;

	    ret = 0;
	    sms_t->stat = 0;
	    at_response_free(p_response);
	#endif
    }

    sms_send_free(pdus);
    return 0;
    
	done:
	    sms_t->stat = SMS_ERROR;
	    at_response_free(p_response);
	    sms_send_free(pdus);
	    return ret;
}

static int sms_init(void *data, unsigned int datalen)
{
    unsigned int i;
    //memcpy(sms_t->receive_number, "15311484723", 11);
    memcpy(sms_t->receive_number, "15201074652", 11);

#if 0
	//sms_receive_printf("0891683110602305F0040D91685175605321F40000512161716000230761F1985C369F01");
	sms_receive_printf("0891683108100045F2240FA101565752110101F10008814052918494234C30106DD85B9D7F51301160A84E8E00320030003100385E740030003467080032003565E575338BF74E86624B673A53F778016CE8518CFF0C68219A8C7801662F003300380037003500310039");

	char *msg = "博宏科元!";
    //char *msg = "abcdefg";
    sms_send_printf("+8613080005005","+8613811420981", msg, NULL);
#endif
    return 0;
}

static int sms_deinit(void *data, unsigned int datalen)
{
    return 0;
}
static int sms_start(void *data, unsigned int datalen)
{
    sms_send_status(data, datalen);
    return 0;
}
static int sms_stop(void *data, unsigned int datalen)
{
    return 0;
}
static int sms_set_info(void *data, unsigned int datalen)
{

    return 0;
}
static int sms_get_info(void *data, unsigned int datalen)
{
    if(!data || datalen != sizeof(struct sms_info))
        return -1;
    memcpy((struct sms_info *)data, sms_t, sizeof(struct sms_info));
    return 0;
}
static int sms_test(void *data, unsigned int datalen)
{
    return 0;
}

static int sms_send_status(void *data, unsigned int datalen)
{
#if 1
    ATResponse *p_response = NULL;
    int err = -1;
    int ret = -1;
    char *smsLine = NULL;
    char *smsResult = NULL;
    char smsCmd[100] = {0};
    int smsPosition;
    char number_string[100];

    /* 设置短信格式为TEXT */
    at_send_command("AT+CMGF=1", NULL);

    /* 设置数据编码为 UCS2*/
    at_send_command("AT+CSMP=,,0,8", NULL);

    /* 中心号码 */
    at_send_command("AT+CSCA?", NULL);

    /* 设置 TE 字符集为 UCS2 编码*/
    at_send_command("AT+CSCS=\"UCS2\"", NULL);

    sprintf(smsCmd, "AT+CMGS=\"%s\"", get_number(number_string, sms_t->receive_number));
    printf("长度: %lu, 发送短信号码： %s\n", strlen(smsCmd), smsCmd);
    err = at_send_command_sms(smsCmd, "535A5B8F79D15143", "+CMGS:", &p_response);
    if (err != 0)
    {
        ret = SMS_ERROR;
        goto done;
    }

    switch (at_get_cme_error(p_response))
    {
    case CME_SUCCESS:
        break;

    default:
        ret = SMS_ERROR;
        goto done;
    }

    /* CMGS has succeeded, now look at the result */

    smsLine = p_response->p_intermediates->line;
    err = at_tok_start(&smsLine);

    if (err < 0)
    {
        ret = SMS_ERROR;
        goto done;
    }

    err = at_tok_nextint(&smsLine, &smsPosition);

    if (err < 0)
    {
        ret = SMS_ERROR;
        goto done;
    }

    at_response_free(p_response);
    p_response = NULL;
    smsResult = NULL;

    ret = 0;
    sms_t->stat = 0;
    at_response_free(p_response);
    return ret;
done:
    sms_t->stat = SMS_ERROR;
    at_response_free(p_response);
    return ret;
#else
	int ret = -1;
	/* pdu方式 */
	ret = sms_send_printf(sms_t->center_number, sms_t->receive_number, "123456789", NULL);
	return ret;	
#endif
}



