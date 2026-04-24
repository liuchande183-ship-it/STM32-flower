#include "sys.h"

static uint8_t InterFace =
    0; //页面显示标志位  自动模式下，一页显示不完所有信息，所以分两页显示，0显示第一页，1显示第二页
uint8_t OperateMode = 0; //运行模式标志位
/******************
功能：模式选择及控制界面
参数：无
返回值：无
******************/
void Mode_Decide(void)//模式判定
{
    //按键判定
    if (isKey1) { //若按键1按下
        isKey1 = isKey2 = isKey3 = isKey4 = 0; //清除按键按下标志
        oled_Clear();
        OperateMode++;
    } else if (isKey2 && (!OperateMode)) { //若按键2按下,且当前模式为自动模式
        isKey1 = isKey2 = isKey3 = isKey4 = 0; //清除按键按下标志
//        oled_Clear();
//        InterFace = !InterFace;
    }

    //标志位限制
    if (OperateMode >= 3) {
        OperateMode = 0;
    }

    if (System.ClearFlag) {
        oled_Clear();
        System.ClearFlag = 0;
    }

    /**根据模式选择显示界面**/
    switch (OperateMode) {
    case 0:
        Inform_Show();//显示信息
        AutoContrl();//自动控制
        break;

    case 1:
        Manual_Contrl();//手动模式
        break;

    case 2:
        ThresholdSet();//阈值设置1
        break;
    }
}

/******************
功能：显示界面
参数：无
返回值：无
******************/
void Inform_Show(void)
{
    if (!InterFace) { //自动模式页面1
        oled_ShowCHinese(16, 0, 14);
        oled_ShowCHinese(32, 0, 15);
        oled_ShowCHinese(48, 0, 16);
        oled_ShowCHinese(64, 0, 17);
        oled_ShowCHinese(80, 0, 18);
        oled_ShowCHinese(96, 0, 19); //大棚环境监测
        oled_ShowCHinese(0, 2, 0);
        oled_ShowCHinese(16, 2, 2);
        OLED_ShowChar(32, 2, ':', 16);//温度
        oled_ShowCHinese(0, 4, 8);
        oled_ShowCHinese(16, 4, 9);
        OLED_ShowChar(32, 4, ':', 16); //
			   oled_ShowCHinese(0, 6, 6);
        oled_ShowCHinese(16, 6, 7);
        OLED_ShowChar(32, 6, ':', 16); //光照
        oled_ShowNum(48, 2, dht11Data.temp_int, 2, 16);
        oled_ShowNum(48, 4, SensorData.SoliVal, 2, 16); 
        oled_ShowNum(48, 6, SensorData.LightVal, 2, 16);
        oled_ShowCHinese(80, 2, 3);
        oled_ShowString(80, 4, "%", 16);
        oled_ShowString(80, 6, "%", 16);
    }
}


/******************
功能：自动控制界面
参数：无
返回值：无
******************/

void AutoContrl(void)
{

    if (dht11Data.temp_int >= Threshold.TempMax) {
         RELAY_ON;
    } else {
         RELAY_OFF;
    }

    if (SensorData.SoliVal <= Threshold.SoliMin) {
       RELAY1_ON; 
    } else if (SensorData.SoliVal > (Threshold.SoliMin + 5)) {
       RELAY1_OFF; 
    }

    // ????
    if (SensorData.LightVal <= Threshold.LightMin) {
        LED1_ON;
    } else {
        LED1_OFF;
    }
}


/******************
功能：手动控制界面
参数：无
返回值：无
******************/
void Manual_Contrl(void)
{
    static uint8_t IndexFlag = 1; //选项索引
    static uint8_t CursorFlag = 1; //光标索引
    //手动模式
    oled_ShowCHinese(32, 0, 20);
    oled_ShowCHinese(48, 0, 21);
    oled_ShowCHinese(64, 0, 22);
    oled_ShowCHinese(80, 0, 23);

    /*按键检测*/
    if (isKey2) {
        IndexFlag++;

        if (IndexFlag == 4) {
            IndexFlag = 1;    //选项个数限制
        }

        if (IndexFlag == 1 || IndexFlag == 4) {
            CursorFlag = 1;
            oled_Clear();
        } else if (IndexFlag == 2 || IndexFlag == 5) {
            CursorFlag = 2;
        } else if (IndexFlag == 3 || IndexFlag == 6) {
            CursorFlag = 3;
        }

        isKey2 = 0;
    }

    if (isKey3) {
        if (IndexFlag == 1) {
            System.Switch1 = !System.Switch1;
        } else if (IndexFlag == 2) {
            System.Switch2 = !System.Switch2;
        } else if (IndexFlag == 3) {
            System.Switch3 = !System.Switch3;
        } 

        isKey3 = 0;
    }

    if (IndexFlag <= 3) { //第一页
        //空调
        oled_ShowCHinese(16, 2, 28);
        oled_ShowCHinese(32, 2, 29);
        OLED_ShowChar(48, 2, ':', 16);
        //加湿
        oled_ShowCHinese(16, 4, 10);
        oled_ShowCHinese(32, 4, 11);
        OLED_ShowChar(48, 4, ':', 16);
        //排风
        oled_ShowCHinese(16, 6, 33);
        oled_ShowCHinese(32, 6, 34);
        OLED_ShowChar(48, 6, ':', 16);

        if (System.Switch1) {
            oled_ShowString(72, 2, "ON ", 16);
        } else {
            oled_ShowString(72, 2, "OFF", 16);
        }

        if (System.Switch2) {
            oled_ShowString(72, 4, "ON ", 16);
        } else {
            oled_ShowString(72, 4, "OFF", 16);
        }

        if (System.Switch3) {
            oled_ShowString(72, 6, "ON ", 16);
        } else {
            oled_ShowString(72, 6, "OFF", 16);
        }
    } 

    switch (CursorFlag) { //光标显示
    case 1:
        OLED_ShowChar(0, 2, '*', 16);
        OLED_ShowChar(0, 4, ' ', 16);
        OLED_ShowChar(0, 6, ' ', 16);
        break;

    case 2:
        OLED_ShowChar(0, 2, ' ', 16);
        OLED_ShowChar(0, 4, '*', 16);
        OLED_ShowChar(0, 6, ' ', 16);
        break;

    case 3:
        OLED_ShowChar(0, 2, ' ', 16);
        OLED_ShowChar(0, 4, ' ', 16);
        OLED_ShowChar(0, 6, '*', 16);
        break;
    }

    if (System.Switch1) {
        RELAY_ON;
    } else {
        RELAY_OFF;
    }

    if (System.Switch2) {
        RELAY1_ON;
    } else {
        RELAY1_OFF;
    }

    if (System.Switch3) {
         LED1_ON;
    } else {
        LED1_OFF;
    }

   

  
}


/******************
功能：阈值设置
参数：无
返回值：无
******************/
void ThresholdSet(void)
{
    static uint8_t IndexFlag1 = 1; //选项索引
    static uint8_t CursorFlag1 = 1; //光标索引
    //阈值设置
    oled_ShowCHinese(32, 0, 24);
    oled_ShowCHinese(48, 0, 25);
    oled_ShowCHinese(64, 0, 26);
    oled_ShowCHinese(80, 0, 27);

    /*按键检测*/
    if (isKey2) {
        IndexFlag1++;

        if (IndexFlag1 == 4) {
            IndexFlag1 = 1;
        }

        if (IndexFlag1 == 1 || IndexFlag1 == 4) {
            CursorFlag1 = 1;
            oled_Clear();
        } else if (IndexFlag1 == 2 || IndexFlag1 == 5) {
            CursorFlag1 = 2;
        } else if (IndexFlag1 == 3 || IndexFlag1 == 6) {
            CursorFlag1 = 3;
        }

        isKey2 = 0;
    } else if (isKey3) {
        isKey3 = 0;

        if (IndexFlag1 == 1) {
            if (Threshold.TempMax < 99) {
                Threshold.TempMax++;
            }
        }  else if (IndexFlag1 == 2) {
            if (Threshold.LightMin < 100) {
                Threshold.LightMin++;
            }
        } else if (IndexFlag1 == 3) {
            if (Threshold.SoliMin < 100) {
                Threshold.SoliMin++;
            }
        } 
    } else if (isKey4) {
        isKey4 = 0;

        if (IndexFlag1 == 1) {
            if (Threshold.TempMax > 0) {
                Threshold.TempMax--;
            }
        } else if (IndexFlag1 == 2) {
            if (Threshold.LightMin > 0) {
                Threshold.LightMin--;
            }
        } else if (IndexFlag1 == 3) {
            if (Threshold.SoliMin > 0) {
                Threshold.SoliMin--;
            }
        } 
    }

    if (IndexFlag1 <= 3) { //第一页
        // 温度上限
        oled_ShowCHinese(16, 2, 0);
        oled_ShowCHinese(32, 2, 2);
        oled_ShowCHinese(48, 2, 37);
        oled_ShowCHinese(64, 2, 39);
        OLED_ShowChar(80, 2, ':', 16);
        // 光照下限
        oled_ShowCHinese(16, 4, 6);
        oled_ShowCHinese(32, 4, 7);
        oled_ShowCHinese(48, 4, 38);
        oled_ShowCHinese(64, 4, 39);
        OLED_ShowChar(80, 4, ':', 16);
        // 土壤下限
        oled_ShowCHinese(16, 6, 8);
        oled_ShowCHinese(32, 6, 9);
        oled_ShowCHinese(48, 6, 38);
        oled_ShowCHinese(64, 6, 39);
        OLED_ShowChar(80, 6, ':', 16);
        // 数值显示
        oled_ShowNum(90, 2, Threshold.TempMax, 2, 16);
        oled_ShowNum(90, 4, Threshold.LightMin, 2, 16);
        oled_ShowNum(90, 6, Threshold.SoliMin, 2, 16);
        // 单位显示
        oled_ShowCHinese(112, 2, 3);
        OLED_ShowChar(112, 4, '%', 16);
        OLED_ShowChar(112, 6, '%', 16);
    } 

    switch (CursorFlag1) { //根据光标索引进行光标显示
    case 1:
        OLED_ShowChar(0, 2, '*', 16);
        OLED_ShowChar(0, 4, ' ', 16);
        OLED_ShowChar(0, 6, ' ', 16);
        break;

    case 2:
        OLED_ShowChar(0, 2, ' ', 16);
        OLED_ShowChar(0, 4, '*', 16);
        OLED_ShowChar(0, 6, ' ', 16);
        break;

    case 3:
        OLED_ShowChar(0, 2, ' ', 16);
        OLED_ShowChar(0, 4, ' ', 16);
        OLED_ShowChar(0, 6, '*', 16);
        break;
    }
 
    //阈值限幅
    if (Threshold.TempMax > 99) {
        Threshold.TempMax = 99;
    }

    if (Threshold.LightMin > 100) {
        Threshold.LightMin = 100;
    }

    if (Threshold.SoliMin >= 100) {
        Threshold.SoliMin = 100;
    }

    if (Threshold.SGP30Max >= 1000) {
        Threshold.SGP30Max = 1000;
    }

}




