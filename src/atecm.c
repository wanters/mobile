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
#include "atecm.h"
#include "atchannel.h"
#include <unistd.h>
#include "at_tok.h"


static int ecm_init(void *data, unsigned int datalen);
static int ecm_deinit(void *data, unsigned int datalen);
static int ecm_start(void *data, unsigned int datalen);
static int ecm_stop(void *data, unsigned int datalen);
static int ecm_set_info(void *data, unsigned int datalen);
static int ecm_get_info(void *data, unsigned int datalen);
static int ecm_test(void *data, unsigned int datalen);
static int ecm_send_status(void *data, unsigned int datalen);

static struct ecm_info  ecminfo;
static struct ecm_info *ecm = &ecminfo;


struct usbev_module usbdev_ecm =
{
    ._init 		= ecm_init,
    ._deinit 	= ecm_deinit,
    ._start 	= ecm_start,
    ._stop 		= ecm_stop,
    ._set_info 	= ecm_set_info,
    ._get_info 	= ecm_get_info,
};

#if 1
static int checkEcmState(void)
{
    ATResponse *p_response = NULL;
    int err = -1;
    char *line = NULL;
    char ret = -1;

    err = at_send_command_singleline("AT^NDISSTATQRY?", "^NDISSTATQRY:", &p_response);

    if (err < 0 || p_response->success == 0)
    {
        goto error;
    }

    line = p_response->p_intermediates->line;
    printf("状态：%s\n", line);
    err = at_tok_start(&line);
    if (err < 0) goto error;

    err = at_tok_nextbool(&line, &ret);
    if (err < 0) goto error;

    at_response_free(p_response);

    return (int)ret;

error:

    at_response_free(p_response);
    return -1;
}

static int requestEcmOnDown(int state)
{
    int 			err = -1;
    int				curr_ecm_state;
    char			at_cmd[64];

    memset(at_cmd, 0, sizeof(at_cmd));

    curr_ecm_state = checkEcmState();
    printf("curr_ecm_state:%d\n", curr_ecm_state);
    if(curr_ecm_state == state)
    {
        return 0;
    }
    printf("状态不一致\n");
    if (curr_ecm_state == ECM_STATE_OFF)
    {
        sprintf(at_cmd, "AT^NDISDUP=1,%d,\"APN\"", state);
    }
    else
    {
        sprintf(at_cmd, "AT^NDISDUP=1,%d", state);
    }

    err = at_send_command(at_cmd, NULL);
    if (checkEcmState() == state)
    {
        printf("状态:%d\n", state);
        ecm->stat = state;
    }
    else
    {
        ecm->stat = curr_ecm_state;
    }

    return err;
}
#endif

static int ecm_init(void *data, unsigned int datalen)
{
    /* 设置ECM */
    printf("ECM INIT\n");
    requestEcmOnDown(ECM_STATE_ON);

    //at_send_command("AT^NDISDUP=1,1,\"APN\"", NULL);
    return 0;
}
static int ecm_deinit(void *data, unsigned int datalen)
{
    return 0;
}
static int ecm_start(void *data, unsigned int datalen)
{
    return 0;
}
static int ecm_stop(void *data, unsigned int datalen)
{
    return 0;
}
static int ecm_set_info(void *data, unsigned int datalen)
{

    return 0;
}
static int ecm_get_info(void *data, unsigned int datalen)
{
    if(!data || datalen != sizeof(struct ecm_info))
        return -1;
    memcpy((struct ecm_info *)data, ecm, sizeof(struct ecm_info));
    return 0;
}


