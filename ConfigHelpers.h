#pragma once

const static char *ConfigID[] = {
	"基本配置",
	"日期时间",
	"夏令时",
	"现场显示",
	"现场通道",
	"主输出",
	"辅助输出",
	"音频",
	"通道",
	"字符叠加",
	"计划编码",
	"报警编码",
	"网络编码",
	"录像配置",
	"录像计划",
	"移动计划",
	"传感器计划",
	"网络基本",
	"网络高级",
	"DDNS",
	"邮箱",
	"用户",
	"传感器配置",
	"传感器触发报警",
	"移动报警配置",
	"移动触发报警",
	"视频丢失触发报警",
	"云台配置",
	"蜂鸣器",
	"视频颜色"
};

const static char *ConfigChoice[] = {
	"NTSC|PAL|",
	"|年月日|月日年|日月年",
	"GMT -12|GMT -11|GMT -10|GMT -9|GMT -8|GMT -7|GMT -6|GMT -5|GMT -4:30|GMT -4|GMT -3:30|GMT -3|GMT -2|GMT -1|GMT 0|GMT +1|GMT +2|GMT +3|GMT +3:30|GMT +4|GMT +4:30|GMT +5|GMT +5:30|GMT +5:45|GMT +6|GMT +6:30|GMT +7|GMT +8|GMT +9|GMT +9:30|GMT +10|GMT +11|GMT +12|",
	"周|日期",
	"一月|二月|三月|四月|五月|六月|七月|八月|九月|十月|十一月|十二月",
	"周日|周一|周二|周三|周四|周五|周六",
	"周|日期",
	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080",
	"|变码流|固定码流|",
	//"QCIF|CIF|HD1|D1|QVGA|VGA|XVGA|QQVGA",
	
	"640x480|720x480|720x576|800x600|1024x768|1280x960|1280x1024|1920x1080|320x240|352x240|480x240|704x480|704x576|960x480|960x576|960x1080|1280x720|1600x1200|1920x1536|2048x1536|2304x1296|2592x1520|2560x1440|2592x1944|3840x2160|352x288",
	"|最低|较低|低|中等|较高|最高",
	"|英语|简体中文|繁体中文|葡萄牙语|希腊语|西班牙语|俄语|挪威语|土耳其语|意大利语|捷克语|德语|希伯来语|日语|法语|波兰语|保加利亚语|印度尼西亚语|俄语D|泰语|匈牙利语|立陶宛语",
	"|12小时制|24小时制",
	"1x1|2x2|2x3|3x3|4x4|5x5|6x6",
	"|88IP|DNS2P|MEIBU|DYNDNS|NOIPDNS|DVRDYDNS|MINTDNS|MYQSEE|DVRLIS|EASTERNDNS|NEWDDNS",
	"常开|常闭",
	"NULL|PELCOP|PELCOD|LILIN|MINKING|NEON|STAR|VIDO|DSCP|VISCA|SAMSUNG|RM110|HY",
	"|管理员|高级用户|普通用户",
};

#define SYSBASIC_HEADER _T("设备名称|设备编号|视频格式|分辨率|刷新率|屏幕保护|系统语言|检查密码|允许覆盖")
#define DATETIME_HEADER _T("日期格式|时间格式|时区|ntp同步|ntp端口|ntp地址")
#define DST_WEEK_HEADER _T("开启|偏移|类型|入-月|入-周|入-星期|入-秒|出-月|出-周|出-星期|出-秒")
#define DST_DATE_HEADER _T("开启|偏移|类型|入-月|入-天|入-秒|出-月|出-天|出-秒")
#define LIVE_HEADER		_T("显示系统时间|显示网络状态|显示硬盘信息|显示USB信息|显示报警输入|显示报警输出")
#define LIVE_CHNN_HEADER _T("显示通道名称|显示录像状态")
#define SPLIT_HEADER_1x1 _T("分裂模式|跳台时间|位置1")
#define SPLIT_HEADER_2x2 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4")
#define SPLIT_HEADER_2x3 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4|位置5|位置6")
#define SPLIT_HEADER_3x3 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4|位置5|位置6|位置7|位置8|位置9")
#define SPLIT_HEADER_4x4 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4|位置5|位置6|位置7|位置8|位置9|位置10|位置11|位置12|位置13|位置14|位置15|位置16")
#define SPLIT_HEADER_5x5 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4|位置5|位置6|位置7|位置8|位置9|位置10|位置11|位置12|位置13|位置14|位置15|位置16|位置17|位置18|位置19|位置20|位置21|位置22|位置23|位置24|位置25")
#define SPLIT_HEADER_6x6 _T("分裂模式|跳台时间|位置1|位置2|位置3|位置4|位置5|位置6|位置7|位置8|位置9|位置10|位置11|位置12|位置13|位置14|位置15|位置16|位置17|位置18|位置19|位置20|位置21|位置22|位置23|位置24|位置25|位置26|位置27|位置28|位置29|位置30|位置31|位置32|位置33|位置34|位置35|位置36")
#define LIVE_AUDIO_HEADER _T("停留时间|音量|通道编号")
#define CHNN_HEADER		 _T("显示通道|通道名称")
#define OSD_HEADER		 _T("通道名称|时间戳|时间戳加星期|自定义文本|通道名称位置|时间戳位置|自定义文本位置|自定义文本")
#define ENCODE_HEADER		 _T("分辨率|帧率|编码|画质|最大码率|最小码率")
#define COLOR_HEADER		 _T("亮度|对比度|色度|饱和度")
#define RECORD_HEADER		 _T("录制视频|录制音频|对应的音频通道|冗余录制|警前时间|警后时间|过期时间")
#define NET_BASIC_HEADER     _T("动态网络地址|网络地址|子网掩码|网关|主域名服务器|从域名服务器|使用PPPoE|帐号|密码")
#define NET_ADVANCE_HEADER     _T("HTTP端口|数据端口|消息命令端口|报警端口|在线用户数目|多播地址|MTU字节数")
#define DDNS_HEADER     _T("启用DDNS|上报周期|服务器类型|是否主机域名|DDNS帐号|DDNS密码|主机域名")
#define SMTP_HEADER     _T("端口|启用SSL|服务器|发送地址|密码|接收地址1|接收地址2|接收地址3")
#define MOTION_SETUP_HEADER     _T("启用|延迟时间|灵敏度|横向网格数|纵向网格数")
#define ALARM_TRIGGER_HEADER     _T("蜂鸣器|大画面|邮件|上传报警中心|报警输出")
#define SENSOR_SETUP_HEADER     _T("启用|设备类型|延迟时间|设备名称")
#define PTZ_SETUP_HEADER     _T("启用|地址|协议|波特率|数据位|停止位|奇偶校验位|数据流控制")
#define BUZZER_HEADER     _T("启用|延迟时间")
#define USER_HEADER     _T("启用|绑定mac|所属|mac|用户名|密码|日志搜索|系统配置|文件管理|磁盘管理|远程登入|语音对讲|系统维护|用户管理|关机重启|报警输出|网络报警|网络串口")



static char *INDEX_CSS = "\
body {\
font: normal 15px auto 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
background: #E6EAE9;\
}\
a {\
color: #c75f3e;\
}\
";

static char *CONFIG_CSS = "\
body {\
font: normal 11px auto 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
background: #E6EAE9;\
}\
a {\
color: #c75f3e;\
}\
table {\
width: 800px;\
padding: 0;\
margin: 0;\
}\
caption {\
padding: 0 0 5px 0;\
width: 700px;\
font: italic 11px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
text-align: right;\
}\
th {\
font: bold 11px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #4f6b72;\
border-right: 1px solid #C1DAD7;\
border-bottom: 1px solid #C1DAD7;\
border-top: 1px solid #C1DAD7;\
letter-spacing: 2px;\
text-transform: uppercase;\
text-align: left;\
padding: 6px 6px 6px 12px;\
background: #CAE8EA no-repeat;\
}\
th.nobg {\
border-top: 0;\
border-left: 0;\
border-right: 1px solid #C1DAD7;\
background: none;\
}\
td {\
border-right: 1px solid #C1DAD7;\
border-bottom: 1px solid #C1DAD7;\
background: #fff;\
font-size:11px;\
padding: 6px 6px 6px 12px;\
color: #4f6b72;\
}\
td.alt {\
background: #F5FAFA;\
color: #797268;\
}\
th.spec {\
border-left: 1px solid #C1DAD7;\
border-top: 0;\
background: #fff no-repeat;\
font: bold 10px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
}\
th.specalt {\
border-left: 1px solid #C1DAD7;\
border-top: 0;\
background: #f5fafa no-repeat;\
font: bold 10px 'Trebuchet MS', Verdana, Arial, Helvetica, sans-serif;\
color: #797268;\
}\
html>body td{ font-size:11px;}\
body,td,th {\
font-family: 宋体, Arial;\
font-size: 12px;\
}";