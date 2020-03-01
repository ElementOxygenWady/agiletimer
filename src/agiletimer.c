/************************************************************************
 * @FileName   ( 文件名 ) : agiletimer.c
 * @CreateDate (创建日期) : 2018/10/08
 * @Author     ( 作  者 ) : Wady(王德宇)
 * @Description( 描  述 ) : 敏捷定时器
 * @Telephone  ( 电  话 ) : 加微信吧
 * @WeChat     ( 微  信 ) : 加QQ吧
 * @QQ         (   QQ   ) : 先评论再加好不好^_^? 1320465765
 * @Instruction( 说  明 ) : 程序可以任意传播或使用! 如有帮助, 请支持, 谢谢!
 ************************************************************************/
#include "agiletimer/agiletimer.h"
#include "./linux/list.h"
#ifdef THREAD_FOUND
    #include <pthread.h>
#endif

#define LOG_TAG                 "AGILETIMER" //日志标签
#define DEF_AGILETIMER_ID       "$AGILETIMER...$" //Do not make changes unless you know what you are doing.

#if (0)
    #define AGILETIMER_ST_LIST_HEAD                             struct list_head
    #define AGILETIMER_ST_LIST_NODE                             struct list_head
    #define AGILETIMER_LIST_INIT_LIST_HEAD                      INIT_LIST_HEAD
    #define AGILETIMER_LIST_ADD                                 list_add
    #define AGILETIMER_LIST_FOR_EACH                            list_for_each_safe
    #define AGILETIMER_LIST_ENTRY                               list_entry
    #define AGILETIMER_LIST_DEL                                 list_del
#else
    #define AGILETIMER_ST_LIST_HEAD                             struct hlist_head
    #define AGILETIMER_ST_LIST_NODE                             struct hlist_node
    #define AGILETIMER_LIST_INIT_LIST_HEAD                      INIT_HLIST_HEAD
    #define AGILETIMER_LIST_ADD                                 hlist_add_head
    #define AGILETIMER_LIST_FOR_EACH                            hlist_for_each_safe
    #define AGILETIMER_LIST_ENTRY                               hlist_entry
    #define AGILETIMER_LIST_DEL                                 hlist_del
#endif

#ifdef THREAD_FOUND
    #define AGILETIMER_LOCK()                                   pthread_mutex_lock(&pstTimer->lock)
    #define AGILETIMER_UNLOCK()                                 pthread_mutex_unlock(&pstTimer->lock)
#else
    #define AGILETIMER_LOCK()
    #define AGILETIMER_UNLOCK()
#endif

static PAgileTimer gspstAgileTimerDefault = NULL; //默认敏捷定时器

/************************************************************************
 * @struct tagAgileTimer
 *      敏捷定时器
 ************************************************************************/
struct tagAgileTimer {
    Char                        szEyeCatcher[16];               //Do not make changes unless you know what you are doing.
    AGILETIMER_ST_LIST_HEAD     stListHead;                     //链表头
    UInt                        uiTimeBase;                     //时基
    UInt                        uiTimeBaseHelper;               //时基助手
#ifdef THREAD_FOUND
    pthread_mutex_t             lock;                           //锁
#endif
};

/************************************************************************
 * @enum tagAgileTimerNodeStatus
 *      敏捷定时器节点状态
 ************************************************************************/
typedef enum tagAgileTimerNodeStatus {
    emAgileTimerNodeRunning     = 0x01,                         //运行
    emAgileTimerNodePending     = 0x02,                         //挂起
    emAgileTimerNodeRefresh     = 0x04,                         //刷新
    emAgileTimerNodeCancel      = 0x08,                         //取消
} AgileTimerNodeStatus;

/************************************************************************
 * @struct tagAgileTimerNode
 *      敏捷定时器节点
 ************************************************************************/
struct tagAgileTimerNode {
    Char *                      pszEyeCatcher1;                 //Do not make changes unless you know what you are doing.
    Char *                      pszEyeCatcher2;                 //Do not make changes unless you know what you are doing.
    Char *                      pszEyeCatcher3;                 //Do not make changes unless you know what you are doing.
    Char *                      pszEyeCatcher4;                 //Do not make changes unless you know what you are doing.
    AGILETIMER_ST_LIST_NODE     stListEntry;                    //链表入口
    PAgileTimer                 pstHolder;                      //持有者
    AgileTimerUserFunc          cbFunc;                         //用户功能函数
    AgileTimerUserFunc          cbDtor;                         //用户析构函数
    void *                      pArg;                           //用户参数
    UInt                        uiRemain;                       //剩余时间(单位: 毫秒)
    UInt                        uiPeriod;                       //周期时间(单位: 毫秒)
    Int                         iTimes;                         //<= 0 - 用户功能函数执行无限次数, 其他 - 用户功能函数执行次数
    AgileTimerNodeStatus        enStatus;                       //状态
};

static Char gszAgileTimerEyeCatcher[] = {DEF_AGILETIMER_ID};

PAgileTimer agiletimer_new(void) {
    PAgileTimer pstTimer = NULL;

    pstTimer = malloc(sizeof(AgileTimer));
    if (pstTimer == NULL) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to malloc holder.");
        return NULL;
    }

    snprintf(pstTimer->szEyeCatcher, sizeof(pstTimer->szEyeCatcher), DEF_AGILETIMER_ID);
    AGILETIMER_LIST_INIT_LIST_HEAD(&pstTimer->stListHead);
    pstTimer->uiTimeBase = 0;
    pstTimer->uiTimeBaseHelper = 0;

#ifdef THREAD_FOUND
    {
        pthread_mutexattr_t attr;
        Int iRet = 0;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE); //必须使用递归锁, 因为用户功能函数内可能再次引用锁
        iRet = pthread_mutex_init(&pstTimer->lock, &attr);
        pthread_mutexattr_destroy(&attr);
        if (iRet) {
            AGILETIMER_LOGE(LOG_TAG, "Failed to init mutex.");
            free(pstTimer);
            return NULL;
        }
    }
#endif

    return pstTimer;
}

void agiletimer_free(PAgileTimer pstTimer) {
    if (pstTimer == NULL)
        pstTimer = gspstAgileTimerDefault;
    AGILETIMER_LOCK();
#ifdef THREAD_FOUND
    pthread_mutex_destroy(&pstTimer->lock);
#endif
    memset(pstTimer->szEyeCatcher, 0, sizeof(pstTimer->szEyeCatcher));
    free(pstTimer);
}

PAgileTimerNode agiletimer_create_node(PAgileTimer pstTimer, Int iTimes, AgileTimerUserFunc cbFunc, AgileTimerUserFunc cbDtor, void *pArg, UInt uiRemain, UInt uiPeriod) {
    PAgileTimerNode pstNode = NULL;

    if (gspstAgileTimerDefault == NULL)
        agiletimer_new();

    if (pstTimer == NULL)
        pstTimer = gspstAgileTimerDefault;

    if (cbFunc == NULL) {
        AGILETIMER_LOGE(LOG_TAG, "Check parameter cbFunc is NULL.");
        return NULL;
    }

    pstNode = malloc(sizeof(AgileTimerNode));
    if (pstNode == NULL) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to malloc node.");
        return NULL;
    }

    pstNode->pszEyeCatcher1 = gszAgileTimerEyeCatcher;
    pstNode->pszEyeCatcher2 = gszAgileTimerEyeCatcher;
    pstNode->pszEyeCatcher3 = gszAgileTimerEyeCatcher;
    pstNode->pszEyeCatcher4 = gszAgileTimerEyeCatcher;
    pstNode->pstHolder = pstTimer;
    pstNode->cbFunc = cbFunc;
    pstNode->cbDtor = cbDtor;
    pstNode->pArg = pArg;
    pstNode->uiRemain = uiRemain;
    pstNode->uiPeriod = uiPeriod;
    pstNode->iTimes = iTimes;
    pstNode->enStatus = emAgileTimerNodeRunning;

    AGILETIMER_LOCK();
    AGILETIMER_LIST_ADD(&pstNode->stListEntry, &pstTimer->stListHead);
    AGILETIMER_UNLOCK();

    return pstNode;
}

static AgileTimerNodeStatus agiletimer_controller_helper(PAgileTimer pstTimer, PAgileTimerNode pstNode) {
    if (pstNode->enStatus & emAgileTimerNodeCancel)
        goto remove_goto;
    if (pstNode->enStatus & emAgileTimerNodePending)
        return emAgileTimerNodePending;
    if (pstNode->enStatus & emAgileTimerNodeRefresh) {
        pstNode->enStatus = emAgileTimerNodeRunning;
        pstNode->uiRemain = pstNode->uiPeriod;
        return emAgileTimerNodeRunning;
    }

    if (pstNode->uiRemain > pstTimer->uiTimeBase) {
        pstNode->uiRemain -= pstTimer->uiTimeBase;
        return emAgileTimerNodeRunning;
    }

    pstNode->cbFunc(pstNode);
    pstNode->uiRemain = pstNode->uiPeriod;

    if (pstNode->enStatus & emAgileTimerNodeCancel)
        goto remove_goto;
    if (pstNode->enStatus & emAgileTimerNodePending)
        return emAgileTimerNodePending;
    if (pstNode->enStatus & emAgileTimerNodeRefresh) {
        pstNode->enStatus = emAgileTimerNodeRunning;
        pstNode->uiRemain = pstNode->uiPeriod;
        return emAgileTimerNodeRunning;
    }

    if (pstNode->iTimes <= 0)
        return emAgileTimerNodeRunning;

    pstNode->iTimes--;
    if (pstNode->iTimes)
        return emAgileTimerNodeRunning;

remove_goto:
    if (pstNode->cbDtor != NULL)
        pstNode->cbDtor(pstNode);

    pstNode->pszEyeCatcher1 = NULL;
    pstNode->pszEyeCatcher2 = NULL;
    pstNode->pszEyeCatcher3 = NULL;
    pstNode->pszEyeCatcher4 = NULL;

    return emAgileTimerNodeCancel;
}

static Int agiletimer_controller_helper_wrapper(const void *cpListData, const void *cpUserData) {
    const PAgileTimerNode cpstNode = (PAgileTimerNode) cpListData;
    const PAgileTimer cpstTimer = (PAgileTimer) cpUserData;
    AgileTimerNodeStatus enStatus;

    enStatus = agiletimer_controller_helper((PAgileTimer) cpstTimer, (PAgileTimerNode) cpstNode);

    if (enStatus & emAgileTimerNodeCancel) {
        AGILETIMER_LIST_DEL(&cpstNode->stListEntry);
        free(cpstNode);
        return 0;
    }

    if (cpstNode->enStatus & emAgileTimerNodePending)
        return 1;

    if (cpstNode->uiRemain < cpstTimer->uiTimeBaseHelper)
        cpstTimer->uiTimeBaseHelper = cpstNode->uiRemain;

    return 1;
}

UInt agiletimer_controller(PAgileTimer pstTimer, UInt uiTimeBase) {
    AGILETIMER_ST_LIST_NODE *pstPos = NULL;
    AGILETIMER_ST_LIST_NODE *pstEntry = NULL;

    if (pstTimer == NULL)
        pstTimer = gspstAgileTimerDefault;

    AGILETIMER_LOCK();

    pstTimer->uiTimeBase = uiTimeBase;
    pstTimer->uiTimeBaseHelper = 0xFFFFFFFF;

    AGILETIMER_LIST_FOR_EACH(pstPos, pstEntry, &pstTimer->stListHead) {
        PAgileTimerNode pstNode = AGILETIMER_LIST_ENTRY(pstPos, AgileTimerNode, stListEntry);
        agiletimer_controller_helper_wrapper(pstNode, pstTimer);
    }

    uiTimeBase = pstTimer->uiTimeBaseHelper;

    AGILETIMER_UNLOCK();

    return uiTimeBase;
}

PAgileTimer agiletimer_node_get_holder(PAgileTimerNode pstNode) {
    PAgileTimer pstTimer = NULL;
    size_t len = 0;

    if (pstNode == NULL) {
        AGILETIMER_LOGE(LOG_TAG, "Check parameter pstNode is NULL.");
        return NULL;
    }

    if (pstNode->pszEyeCatcher1 != gszAgileTimerEyeCatcher) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to check pstNode->pszEyeCatcher1.");
        return NULL;
    }
    if (pstNode->pszEyeCatcher2 != gszAgileTimerEyeCatcher) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to check pstNode->pszEyeCatcher2.");
        return NULL;
    }
    if (pstNode->pszEyeCatcher3 != gszAgileTimerEyeCatcher) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to check pstNode->pszEyeCatcher3.");
        return NULL;
    }
    if (pstNode->pszEyeCatcher4 != gszAgileTimerEyeCatcher) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to check pstNode->pszEyeCatcher4.");
        return NULL;
    }

    pstTimer = pstNode->pstHolder;

    len = strlen(DEF_AGILETIMER_ID);
    if (memcmp(pstTimer->szEyeCatcher, DEF_AGILETIMER_ID, len)) {
        AGILETIMER_LOGE(LOG_TAG, "Failed to check pstTimer->szEyeCatcher.");
        return NULL;
    }

    return pstTimer;
}

static PAgileTimerNode agiletimer_list_find(PAgileTimer pstTimer, PAgileTimerNode pstNode) {
    AGILETIMER_ST_LIST_NODE *pstPos = NULL;
    AGILETIMER_ST_LIST_NODE *pstEntry = NULL;
    PAgileTimerNode pstNodeRef = NULL;

    AGILETIMER_LOCK();
    AGILETIMER_LIST_FOR_EACH(pstPos, pstEntry, &pstTimer->stListHead) {
        pstNodeRef = AGILETIMER_LIST_ENTRY(pstPos, AgileTimerNode, stListEntry);
        if (pstNode == pstNodeRef)
            break;
    }
    AGILETIMER_UNLOCK();

    if (pstNode == pstNodeRef)
        return pstNode;

    return NULL;
}

void *agiletimer_node_get_user_argument(PAgileTimerNode pstNode) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    void *pArg = NULL;
    if (pstTimer == NULL)
        return NULL;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        pArg = pstNode->pArg;
    AGILETIMER_UNLOCK();
    return pArg;
}

void agiletimer_node_set_user_argument(PAgileTimerNode pstNode, void *pArg) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    if (pstTimer == NULL)
        return;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        pstNode->pArg = pArg;
    AGILETIMER_UNLOCK();
}

UInt agiletimer_node_get_remain(PAgileTimerNode pstNode) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    UInt uiRemain = 0;
    if (pstTimer == NULL)
        return 0;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        uiRemain = pstNode->uiRemain;
    AGILETIMER_UNLOCK();
    return uiRemain;
}

void agiletimer_node_set_remain(PAgileTimerNode pstNode, UInt uiRemain) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    if (pstTimer == NULL)
        return;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        pstNode->uiRemain = uiRemain;
    AGILETIMER_UNLOCK();
}

UInt agiletimer_node_get_period(PAgileTimerNode pstNode) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    UInt uiPeriod = 0;
    if (pstTimer == NULL)
        return 0;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        uiPeriod = pstNode->uiPeriod;
    AGILETIMER_UNLOCK();
    return uiPeriod;
}

void agiletimer_node_set_period(PAgileTimerNode pstNode, UInt uiPeriod) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    if (pstTimer == NULL)
        return;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL)
        pstNode->uiPeriod = uiPeriod;
    AGILETIMER_UNLOCK();
}

static Int agiletimer_node_set_status(PAgileTimerNode pstNode, AgileTimerNodeStatus enStatus) {
    PAgileTimer pstTimer = agiletimer_node_get_holder(pstNode);
    Int iRet = -1;
    if (pstTimer == NULL)
        return -1;
    AGILETIMER_LOCK();
    if (agiletimer_list_find(pstTimer, pstNode) != NULL) {
        pstNode->enStatus = enStatus;
        iRet = 0;
    }
    AGILETIMER_UNLOCK();
    return iRet;
}

Int agiletimer_node_cancel(PAgileTimerNode pstNode) {
    return agiletimer_node_set_status(pstNode, emAgileTimerNodeCancel);
}

Int agiletimer_node_refresh(PAgileTimerNode pstNode) {
    return agiletimer_node_set_status(pstNode, emAgileTimerNodeRefresh);
}

Int agiletimer_node_pause(PAgileTimerNode pstNode) {
    return agiletimer_node_set_status(pstNode, emAgileTimerNodePending);
}

Int agiletimer_node_resume(PAgileTimerNode pstNode) {
    return agiletimer_node_set_status(pstNode, emAgileTimerNodeRunning);
}

/************************************************************************
 * @FunctionName( 函数名 ): Int agiletimer_init(void *pArg)
 * @CreateDate  (创建日期): 2018/10/08
 * @Author      ( 作  者 ): Wady
 *
 * @Description(描述): 初始化敏捷定时器
 *
 * @Param(参数):
 *      pArg                    ~ NULL;
 *
 * @ReturnCode(返回值): 0 - 成功, 其他 - 失败
 *------------------------------------------------------------------------*@RevisionHistory(变更)
 *  None(无)
 ************************************************************************/
Int agiletimer_init(void *pArg) {
    gspstAgileTimerDefault = agiletimer_new();
    if (gspstAgileTimerDefault == NULL)
        return -1;
    return 0;
}
/**@END! Int agiletimer_init(void *pArg) /~ 结束咯 */
