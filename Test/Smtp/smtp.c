#include <CommonDef.h>
#include <CommonInclude.h>
#include <Common.h>
#include <System.h>
#include "Smtp.h"

int main(int artc, char **argv)
{
    S8 s8EmailTo[128] = {"tango_zhu@126.com"};/* 邮件发送地址 */
    S8 s8EmailFrom[128] = {"tango_zhu@163.com"};/* 邮件发送账户 */
    S8 s8EmailPwd[128]= {"3306492351"}; /* 邮件发送账户密码 */
    S8 s8EmailTitle[128]={"test"};/* 邮件标题 */
    S8 s8EmailBody[1024]={"It's an email test with two jpeg picture!"};/* 邮件内容 */ 
    S8 s8EmailAccount[128] ={"tango_zhu"};
    S8 s8AttachName[32] = {"9.jpg"};    
    S8 s8AttachPath[32] = {"./9.jpg"};
    S8 s8AttachName1[32] = {"8.jpg"};    
    S8 s8AttachPath1[32] = {"./8.jpg"};
    int i;
    EMAIL_INFO_STRU stEmailInfo;
    ATTACHMENT_ARRAY_STRU stAttach;
    stEmailInfo.ps8EmailBody = s8EmailBody;
    stEmailInfo.ps8EmailFrom = s8EmailFrom;
    stEmailInfo.ps8EmailPwd = s8EmailPwd;
    stEmailInfo.ps8EmailTitle = s8EmailTitle;
    stEmailInfo.ps8EmailTo = s8EmailTo;   
    stEmailInfo.ps8EmailAccount = s8EmailAccount;
    memset(&stAttach, 0, sizeof(ATTACHMENT_ARRAY_STRU));
    stAttach.stAttachment[0].ps8FileName = s8AttachName;
    stAttach.stAttachment[0].ps8FilePath = s8AttachPath;
    stAttach.stAttachment[1].ps8FileName = s8AttachName1;
    stAttach.stAttachment[1].ps8FilePath = s8AttachPath1;
    stAttach.u32AttachSum = 2;

   while(1)
   {
        i ++;
        printf("send email %d times \n",i);
        SendEmail(&stEmailInfo, &stAttach);
        sleep(1800);
    
   }   
   return 0;
}


