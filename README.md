# pxProcessMemoryMonitor
Monitor the memory of the process and write it to file for analyze.
## 程序功能
1. 该程序可监控指定程序，并在界面中显示该进程所占用的内存数据，包括Commit Memory和Current Working Set.

2. 内存占用数据可实时刷新（目前暂为500ms刷新一次）.

3. 内存数据同时保存在.txt文件中，可利用该文件对进程各时间段的内存占用情况进行分析。
   可辅助分析程序的是否一直在内存泄漏.

