

识别引擎:
    host=root@10.10.10.158
    dir_asr=/home/szm/cd/kaldi-master/test


112 上配置的 真正使用流程：
    当前目录下的 run.sh + run_dynamic_match.remote 
    其中 vad 和 match 模块都需要源码重新在112上编译的


3. dynamic_match
    小语音文本  大语音文本
    动态对齐的过程 

4. run_dynamic_match
    集成 123 的流程 


#### -----------------------------------------------------


#
#使用UTF8函数
#忽略掉一些特殊字符: 空格 标点 等
#转换一下特殊字符

v1.0 
    基础的lcs 测试英文字符串 
v2.0 
    路径跟踪 计算插入 删除 替换错误个数 
    最终对齐后的结果输出 


