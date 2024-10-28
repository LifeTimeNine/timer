# Timer
一个定时任务管理服务
> 为什么会有这个项目？
> 基于所任职公司现有的项目，经常会有对`crontab`编辑和手动执行的场景，觉得每次都使用`vi`或`vim`编辑器进行编辑，然后再去手动执行比较麻烦，无法实现分类管理，日志统计以及结果通知等需求。
> 因此`timer`诞生了

## 安装和启动
在项目根目录执行
~~~shell
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release .. && make install
~~~
默认安装目录是`/usr/local`

如果需要指定安装目录
~~~shell
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/timer .. && make install
~~~

启动
~~~shell
/usr/local/bin/timer
~~~
指定配置文件启动
~~~shell
/usr/local/bin/timer -c /usr/local/etc/timer.ini
~~~

## 以系统服务的方式运行
编辑 `/etc/systemd/system/timer.service`文件  
添加以下内容:  
~~~
[Unit]
Description = timer
After = network.target syslog.target
Wants = network.target

[Service]
Type = simple
ExecStart =/usr/local/bin/timer -c /usr/local/etc/timer.ini
User = root
Group = root
Restart = always

[Install]
WantedBy = multi-user.target
~~~

应用服务并设置开机自启
~~~shell
systemctl daemon-reload
systemctl enable timer
~~~

管理服务  
启动: `systemctl start panel`  
停止: `systemctl stop panel`  
重启: `systemctl restart panel`  
重载配置: `systemctl reload panel`


## 配置

- `main.pid_file` Pid文件存储路径
- `main.notify_url` 事件通知地址(仅支持http, 如果需要https建议使用nginx做一个代理)
- `http.host` HTTP服务监听地址
- `http.port` HTTP服务监听端口
- `log.dir` 日志存储目录
- `log.level` 日志等级 0-trace, 1-debug, 2-info, 3-warn, 4-err, 5-critical 6-off
- `db.path` 数据库文件路径

## 管理接口说明

### 状态码说明
- `0`: 正常
- `1001`: 任务不存在
- `1002`: 参数解析错误

### 获取状态

- 请求方法: `GET`
- 请求地址: `/`
- 请求参数: 无
- 返回结果
  + `running_number`: (int) 正在运行的任务数量
  + `task_total`: (int) 任务总数
  
### 保存任务
根据`uuid`判断，如果存在修改现有的信息，如果不存在保存新的任务

- 请求方法: `POST`
- 请求地址: `/task`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
  + `exec_file`: (string) 可执行文件地址
  + `args`: (string) 参数
  + `loop`: (bool) 是否循环
  + `enable` (bool) 是否启用
  + `cron`: (string) cron表达式(支持 `* * * * * *` 和 `* * * * *`, 如果不设置秒， 默认每分钟第0秒执行)
- 返回结果: 无

### 获取任务列表

- 请求方法: `GET`
- 请求地址: `/task`
- 请求参数: 无
- 返回结果
  + `[].uuid`: (string) 唯一标识
  + `[].exec_file`: (string) 可执行文件地址
  + `[].args`: (string) 参数
  + `[].loop`: (bool) 是否循环
  + `[].enable` (bool) 是否启用
  + `[].cron`: (string) cron表达式

### 获取任务详情

- 请求方法: `GET`
- 请求地址: `/task`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
- 返回结果
  + `uuid`: (string) 唯一标识
  + `exec_file`: (string) 可执行文件地址
  + `args`: (string) 参数
  + `loop`: (bool) 是否循环
  + `enable` (bool) 是否启用
  + `cron`: (string) cron表达式

### 删除任务

- 请求方法: `DELETE`
- 请求地址: `/task`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
- 返回结果: 无

### 运行任务

- 请求方法: `POST`
- 请求地址: `/run`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
- 返回结果: 无

## 通知事件说明

### 开始执行

- 请求方法: `POST`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
  + `start_time`: (string) 开始运行时间
  + `next_run_time`: (string) 下一次运行时间

### 执行结束
- 请求方法: `POST`
- 内容类型: `application/json`
- 请求参数:
  + `uuid`: (string) 唯一标识
  + `start_time`: (string) 开始运行时间
  + `end_time`: (string) 结束运行时间
  + `runtime`: (string) 运行时长(秒)
  + `is_normal_exit`: (bool) 是否正常退出
  + `out`: (string) 标准输出内容
  + `error`: (string) 异常输出内容