/************************************************************************
 * @FileName   ( 文件名 ) : tester.c
 * @CreateDate (创建日期) : 2018/10/08
 * @Author     ( 作  者 ) : Wady(王德宇)
 * @Description( 描  述 ) : 敏捷定时器测试
 * @Telephone  ( 电  话 ) : 加微信吧
 * @WeChat     ( 微  信 ) : 加QQ吧
 * @QQ         (   QQ   ) : 先评论再加好不好^_^? 1320465765
 * @Instruction( 说  明 ) : 程序可以任意传播或使用! 如有帮助, 请支持, 谢谢!
 ************************************************************************/
#include "agiletimer/agiletimer_pub.h"
#include "agiletimer/agiletimer.h"
#include <unistd.h>

#define LOG_TAG                 "TESTER" //日志标签

void *agiletimer_status(PAgileTimerNode pstNode) {
    static Int siCnt = 0;
    AGILETIMER_LOGI(LOG_TAG, "Running... %d", ++siCnt);
    return NULL;
}

void *agiletimer_test4(PAgileTimerNode pstNode) {
    static Int siCnt = 0;
    if ((++siCnt) > 100) {
        AGILETIMER_LOGI(LOG_TAG, "Test4 Running...");
        siCnt = 0;
    }
    return NULL;
}

void *agiletimer_test3(PAgileTimerNode pstNode) {
    static Int siCnt = 0;
    AGILETIMER_LOGI(LOG_TAG, "Error Error Error Error Error Error Error Error Test3 Running... %d", ++siCnt);
    return NULL;
}

void *agiletimer_test2_func(PAgileTimerNode pstNode) {
    PAgileTimerNode pstNodeNew = agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test3, NULL, NULL, 0, 50);
    agiletimer_node_cancel(pstNodeNew);
    return NULL;
}

void *agiletimer_test2_dtor(PAgileTimerNode pstNode) {
    static Int siCnt = 0;
    AGILETIMER_LOGI(LOG_TAG, "Test2 Running... %d", ++siCnt);
    return NULL;
}

void *agiletimer_test1(PAgileTimerNode pstNode) {
    static Int siCnt = 0;
    UInt uiPeriod = 0;

    if ((++siCnt) > 20) {
        agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test2_func, &agiletimer_test2_dtor, NULL, 0, 50);
        siCnt = 0;
    }

    uiPeriod = agiletimer_node_get_period(pstNode);
    if (uiPeriod > 10)
        uiPeriod -= 10;

    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test4, NULL, NULL, 0, uiPeriod);

    return NULL;
}

void *agiletimer_main(PAgileTimerNode pstNode) {
    agiletimer_create_node(agiletimer_node_get_holder(pstNode), 1, &agiletimer_test1, NULL, NULL, 0, 50);
    return NULL;
}

Int main(Int iArgc, Char *pszArgv[], Char *pszEnv[]) {
#ifdef TOOLBOX_FOUND
    toolbox_logging_level_set(NULL, emToolboxLoggingLevelDebug);
#endif
    AGILETIMER_LOGD(LOG_TAG, "Test debug log.");
    AGILETIMER_LOGT(LOG_TAG, "Test trace log.");
    AGILETIMER_LOGI(LOG_TAG, "Test info  log.");
    AGILETIMER_LOGW(LOG_TAG, "Test warn  log.");
    AGILETIMER_LOGE(LOG_TAG, "Test error log.");

    agiletimer_init(NULL);

    agiletimer_create_node(NULL, -1, &agiletimer_status, NULL, NULL, 1000, 1000);
    agiletimer_create_node(NULL, -1, &agiletimer_main, NULL, NULL, 0, 50);

    while (1) {
        usleep(10 * 1000);
        agiletimer_controller(NULL, 10);
    }

    return 0;
}
