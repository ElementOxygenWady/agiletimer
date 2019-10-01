# 敏捷timer
1. 未使用静态变量或全局变量, 所有函数可重入;
2. 核心算法支持Hash List和Double Circular List, 对于MCU应用不能使用Hash List;
3. 控制器每一次调度完成会返回下一次触发的时间, 根据该时间可以优化运行效率;

## 说明：

> **Note:** 不适用实时系统

## 编译及运行方式：
$ `cd agiletimer`<br/>
$ `mkdir output`<br/>
$ `cd output`<br/>
$ `cmake .. -DENABLE_TESTS=ON`<br/>
$ `make`<br/>
$ `./tester/tester_agiletimer`<br/>
