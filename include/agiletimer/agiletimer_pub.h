/************************************************************************
 * @FileName   ( 文件名 ) : agiletimer_pub.h
 * @CreateDate (创建日期) : 2018/10/08
 * @Author     ( 作  者 ) : Wady(王德宇)
 * @Description( 描  述 ) : 敏捷定时器公共
 * @Telephone  ( 电  话 ) : 加微信吧
 * @WeChat     ( 微  信 ) : 加QQ吧
 * @QQ         (   QQ   ) : 先评论再加好不好^_^? 1320465765
 * @Instruction( 说  明 ) : 程序可以任意传播或使用! 如有帮助, 请支持, 谢谢!
 ************************************************************************/
#ifndef __AGILETIMER_PUB_H_
#define __AGILETIMER_PUB_H_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#ifdef TOOLBOX_FOUND
    #include "toolbox/toolbox_logging.h"
#endif

#ifndef Char
#define Char                                                    char                            // 8BIT
#endif

#ifndef UChar
#define UChar                                                   unsigned char                   // 8BIT
#endif

#ifndef Short
#define Short                                                   short                           //16BIT
#endif

#ifndef UShort
#define UShort                                                  unsigned short                  //16BIT
#endif

#ifndef Int
#define Int                                                     int                             //32BIT
#endif

#ifndef UInt
#define UInt                                                    unsigned int                    //32BIT
#endif

#ifndef Long
#define Long                                                    long                            //32BIT
#endif

#ifndef ULong
#define ULong                                                   unsigned long                   //32BIT
#endif

#ifndef LLong
#define LLong                                                   long long                       //64BIT
#endif

#ifndef ULLong
#define ULLong                                                  unsigned long long              //64BIT
#endif

#ifndef Float
#define Float                                                   float                           //32BIT
#endif

#ifndef Double
#define Double                                                  double                          //64BIT
#endif

#ifndef Bool
#define Bool                                                    Int
#endif

#ifndef True
#define True                                                    1
#endif

#ifndef False
#define False                                                   (!True)
#endif

#ifndef NULL
#define NULL                                                    ((void *) 0)
#endif

#ifndef DEF_WRAP
#define DEF_WRAP                                                "\n"
#endif

#define AGILETIMER_LOG_LABEL                                    "Agiletimer" //日志标签

#ifdef TOOLBOX_FOUND
    #ifdef ENABLE_DEBUG
        #define AGILETIMER_LOGD(TAG, P, Q...)                   TOOLBOX_LOGGING_COMMON_DEBUG(AGILETIMER_LOG_LABEL, "[" TAG "] " P, ##Q)
        #define AGILETIMER_LOGT(TAG, P, Q...)                   TOOLBOX_LOGGING_COMMON_TRACE(AGILETIMER_LOG_LABEL, "[" TAG "] " P, ##Q)
    #else
        #define AGILETIMER_LOGD(TAG, P, Q...)
        #define AGILETIMER_LOGT(TAG, P, Q...)
    #endif
    #define AGILETIMER_LOGI(TAG, P, Q...)                       TOOLBOX_LOGGING_COMMON_INFO(AGILETIMER_LOG_LABEL,  "[" TAG "] " P, ##Q)
    #define AGILETIMER_LOGW(TAG, P, Q...)                       TOOLBOX_LOGGING_COMMON_WARN(AGILETIMER_LOG_LABEL,  "[" TAG "] " P, ##Q)
    #define AGILETIMER_LOGE(TAG, P, Q...)                       TOOLBOX_LOGGING_COMMON_ERROR(AGILETIMER_LOG_LABEL, "[" TAG "] " P, ##Q)
    #define AGILETIMER_LOGX(TAG, P, Q...)                       TOOLBOX_LOGGING_COMMON_FATAL(AGILETIMER_LOG_LABEL, "[" TAG "] " P, ##Q)
#else
    #ifdef ENABLE_DEBUG
        #define AGILETIMER_LOGD(TAG, P, Q...)                   printf("D/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
        #define AGILETIMER_LOGT(TAG, P, Q...)                   printf("T/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
    #else
        #define AGILETIMER_LOGD(TAG, P, Q...)
        #define AGILETIMER_LOGT(TAG, P, Q...)
    #endif
    #define AGILETIMER_LOGI(TAG, P, Q...)                       printf("I/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
    #define AGILETIMER_LOGW(TAG, P, Q...)                       printf("W/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
    #define AGILETIMER_LOGE(TAG, P, Q...)                       printf("E/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
    #define AGILETIMER_LOGX(TAG, P, Q...)                       printf("*/" AGILETIMER_LOG_LABEL ": [" TAG "] " P DEF_WRAP, ##Q)
#endif

#endif
