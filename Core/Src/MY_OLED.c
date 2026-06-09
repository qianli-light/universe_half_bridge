//
// Created by wnywl on 2026/5/18.
//

#include "MY_OLED.h"

#include "main.h"
#include "OLED.h"
#include "OLED_Data.h"
#include "stm32g4xx_hal.h"

void BUCK_CC_interface_head(void) {
    OLED_Clear();

    OLED_Printf(0,0,OLED_6X8,"恒流控制 BUCK_CC");
    OLED_ShowString(0,8,"setpoint:",OLED_6X8);
    OLED_ShowChar(84,8,'A',OLED_6X8);
    OLED_ShowString(0,16,"Vl:",OLED_6X8);
    OLED_ShowChar(54,24,'V',OLED_6X8);
    OLED_ShowString(64,16,"Il:",OLED_6X8);
    OLED_ShowChar(112,24,'A',OLED_6X8);

    OLED_Update();
}

void BUCK_CC_interface_main(void) {
    OLED_ShowFloatNum(18,24,phy_V_low,2,2,OLED_6X8);
    OLED_ShowFloatNum(82,24,phy_I_low,1,2,OLED_6X8);

    OLED_Update();
}

void BUCK_CV_interface_head(void) {
    OLED_Clear();

    OLED_Printf(0,0,OLED_6X8,"恒压控制 BUCK_CV");
    OLED_ShowString(0,8,"setpoint:",OLED_6X8);
    OLED_ShowChar(84,8,'V',OLED_6X8);
    OLED_ShowString(0,16,"Vo:",OLED_6X8);
    OLED_ShowChar(54,24,'V',OLED_6X8);
    OLED_ShowString(64,16,"Io:",OLED_6X8);
    OLED_ShowChar(112,24,'A',OLED_6X8);

    OLED_Update();
}

void BUCK_CV_interface_main(void) {
    OLED_ShowFloatNum(18,24,phy_V_low,2,2,OLED_6X8);
    OLED_ShowFloatNum(82,24,phy_I_low,1,2,OLED_6X8);

    OLED_Update();
}

void BOOST_CV_interface_head(void) {
    OLED_Clear();

    OLED_Printf(0,0,OLED_6X8,"恒压控制 BOOST_CV");
    OLED_ShowString(0,8,"setpoint:",OLED_6X8);
    OLED_ShowChar(84,8,'V',OLED_6X8);
    OLED_ShowString(0,16,"Vh:",OLED_6X8);
    OLED_ShowChar(54,24,'V',OLED_6X8);
    OLED_ShowString(64,16,"Ih:",OLED_6X8);
    OLED_ShowChar(112,24,'A',OLED_6X8);

    OLED_Update();
}

void BOOST_CV_interface_main(void) {
    OLED_ShowFloatNum(18,24,phy_V_high,2,2,OLED_6X8);
    OLED_ShowFloatNum(82,24,phy_I_high,1,2,OLED_6X8);

    OLED_Update();
}
void DeBug_interface_head(void) {
    OLED_ShowString(0,32,"OKp:",OLED_6X8);
    OLED_ShowString(64,32,"IKp:",OLED_6X8);
    OLED_ShowString(0,40,"OKi:",OLED_6X8);
    OLED_ShowString(64,40,"IKi:",OLED_6X8);
    OLED_ShowString(0,48,"OKd:",OLED_6X8);
    OLED_ShowString(64,48,"IKd:",OLED_6X8);
    OLED_ShowString(0,56,"EC_DeBug:",OLED_6X8);

    OLED_Update();
}
void DeBug_interface_main(void) {
    OLED_ShowFloatNum(24,32,pid_outer.Kp,2,2,OLED_6X8);
    OLED_ShowFloatNum(88,32,pid_inner.Kp,2,2,OLED_6X8);
    OLED_ShowFloatNum(24,40,pid_outer.Ki,2,2,OLED_6X8);
    OLED_ShowFloatNum(88,40,pid_inner.Ki,2,2,OLED_6X8);
    OLED_ShowFloatNum(24,48,pid_outer.Kd,2,2,OLED_6X8);
    OLED_ShowFloatNum(88,48,pid_inner.Kd,2,2,OLED_6X8);

    switch (now_EC_DeBug) {
        case _phy_setpoint:
            OLED_ShowString(54,56,"phy_setpoint",OLED_6X8);
            break;
        case _OKp:
            OLED_ShowString(54,56,"OKp",OLED_6X8);
            break;
        case _OKi:
            OLED_ShowString(54,56,"OKi",OLED_6X8);
            break;
        case _OKd:
            OLED_ShowString(54,56,"OKd",OLED_6X8);
            break;
        case _IKp:
            OLED_ShowString(54,56,"IKp",OLED_6X8);
            break;
        case _IKi:
            OLED_ShowString(54,56,"IKi",OLED_6X8);
            break;
        case _IKd:
            OLED_ShowString(54,56,"IKd",OLED_6X8);
            break;
        default:
            break;
    }

    OLED_Update();
}

