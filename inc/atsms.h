
#define NUMBER_LENGTH 11

typedef enum
{
    SMS_ERROR		= 1,

} SMS_Status;


struct sms_info
{
    int	    stat;
    char 	receive_number[12];  //电话号码
    int 	format;  //短信格式
    char center_number[20];
};

extern struct sms_info  smsinfo;
extern struct sms_info *sms_t;

void sms_receive_printf(const char *sms_messsge);


