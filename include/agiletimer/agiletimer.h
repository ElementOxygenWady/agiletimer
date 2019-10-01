/************************************************************************
 * @FileName   ( 文件名 ) : agiletimer.h
 * @CreateDate (创建日期) : 2018/10/08
 * @Author     ( 作  者 ) : Wady(王德宇)
 * @Description( 描  述 ) : 敏捷定时器
 * @Telephone  ( 电  话 ) : 加微信吧
 * @WeChat     ( 微  信 ) : 加QQ吧
 * @QQ         (   QQ   ) : 先评论再加好不好^_^? 1320465765
 * @Instruction( 说  明 ) : 程序可以任意传播或使用! 如有帮助, 请支持, 谢谢!
 ************************************************************************/
#ifndef __AGILETIMER_H_
#define __AGILETIMER_H_

#include "./agiletimer_pub.h"

typedef struct tagAgileTimer AgileTimer, *PAgileTimer;
typedef struct tagAgileTimerNode AgileTimerNode, *PAgileTimerNode;

/************************************************************************
 * @Description(描述): 用户函数
 * @Param(参数):
 *      pstNode                 ~ 敏捷定时器节点;
 * @ReturnCode(返回值): 未定义
 ************************************************************************/
typedef void *(*AgileTimerUserFunc)(PAgileTimerNode pstNode);

PAgileTimer agiletimer_new(void);

void agiletimer_free(PAgileTimer pstTimer);

PAgileTimerNode agiletimer_create_node(PAgileTimer pstTimer, Int iTimes, AgileTimerUserFunc cbFunc, AgileTimerUserFunc cbDtor, void *pArg, UInt uiRemain, UInt uiPeriod);

UInt agiletimer_controller(PAgileTimer pstTimer, UInt uiTimeBase);

PAgileTimer agiletimer_node_get_holder(PAgileTimerNode pstNode);

void *agiletimer_node_get_user_argument(PAgileTimerNode pstNode);

void agiletimer_node_set_user_argument(PAgileTimerNode pstNode, void *pArg);

UInt agiletimer_node_get_remain(PAgileTimerNode pstNode);

void agiletimer_node_set_remain(PAgileTimerNode pstNode, UInt uiRemain);

UInt agiletimer_node_get_period(PAgileTimerNode pstNode);

void agiletimer_node_set_period(PAgileTimerNode pstNode, UInt uiPeriod);

Int agiletimer_node_cancel(PAgileTimerNode pstNode);

Int agiletimer_node_refresh(PAgileTimerNode pstNode);

Int agiletimer_node_pause(PAgileTimerNode pstNode);

Int agiletimer_node_resume(PAgileTimerNode pstNode);

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
Int agiletimer_init(void *pArg);
/**@END! Int agiletimer_init(void *pArg) /~ 结束咯 */

#endif
