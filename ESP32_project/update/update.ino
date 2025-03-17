//编译环境：Arduino IDE 2.2.2
#define BLINKER_WIFI //定义wifi模块
#define BLINKER_WITHOUT_SSL //定义通信不加密

#include <Blinker.h>//包含Blinker头文件

char auth[] = "b901e8ab1e96";  //你的设备key
char ssid[] = "Asmedia";  //你的wifi 名称
char pswd[] = "87654321";  //你的wifi 密码

//新建数据类型组件对象，作用：将数据传输到手机blinker app
BlinkerNumber DISTANCE("distance");    //定义数据键名
BlinkerNumber LIGHT("light");    //定义数据键名

int distance_read = 0, light_read = 0;//定义全局变量 储存传感器读取的数据

void StrExtFloat(int *num, char* Str);

void heartbeat()
{
    DISTANCE.print(distance_read);        //给blinkerapp回传数据
    LIGHT.print(light_read);        //给blinkerapp回传数据
}

void setup()//将设置代码放在此处，运行一次；
{
    //初始化端口
    Serial.begin(115200);

    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();

    Blinker.begin(auth, ssid, pswd); //初始化blinker 
    Blinker.attachHeartbeat(heartbeat);//将传感器获取的数据传给blinker app上
}
//通过循环不断读取传感器获取的数据
void loop() //把主代码放在这里，重复运行：
{
  char input[11];
  int indexc = 0;
  int temp_num[3] = {0,0,0};
	while (Serial.available() > 0)  //串口接收数据
  {
    char c = Serial.read();
		if(c == '\n')
		{
			input[indexc] = '\0';
			indexc = 0;
		}
		else
		{
			input[indexc] = c;
			indexc++;
		}
	}
	StrExtFloat(temp_num, input); //处理接收的数据
	
  {
      distance_read = temp_num[0];//将读取到的赋值给全局变量
      light_read = temp_num[1];//将读取到的赋值给全局变量
      //打印
      BLINKER_LOG("Distance: ", distance_read, " cm");
      BLINKER_LOG("Light: ", light_read, " Lux");
      
  }

  Blinker.run();//运行Blinker
  Blinker.delay(1000);//延时函数
}

//数据提取
void StrExtFloat(int *num, char* Str)
{
	char Fnum = 0;	//数字个数
	char num_start = 0;
	
	while ( *Str != '\0' )				//遍历到字符串尾部
	{
		if(*Str >='0' && *Str <= '9')	//判断是不是数字
		{
			num_start = 1;				//判断数字存在
			
			num[Fnum] = num[Fnum]*10+(*Str - '0');	//加入整数部分
		}
		else 							//判断为其他字符
		{
			if (num_start == 1)
			{
				Fnum++;					//统计个数加一
			}
			
			num_start = 0;				//清空字符统计
		}
		
		Str++;							//指针移动
	}
	if (num_start == 1)					//如果不是以字符结尾
	{
		Fnum++;							//统计个数加一
	}
}
