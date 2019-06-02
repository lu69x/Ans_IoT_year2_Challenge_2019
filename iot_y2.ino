#include "Arduino.h"
#include "wiring_private.h"
#include <Wire.h>
#include "PMS.h"

void        Set_Config1             (void);
void        SERCOM1_Handler         (void);
void        Serial2_String_Write    (char *s);
void        Set_ConfigABP_A         (void);
String      Send_Cmd                (char *s);

Uart Serial2(&sercom1, 11, 10, SERCOM_RX_PAD_0, UART_TX_PAD_2);
Uart Serial3(&sercom2, 5, 22, SERCOM_RX_PAD_3, UART_TX_PAD_0);

PMS pms(Serial3);
PMS::DATA pmdata;

void SERCOM1_Handler(void)
{
  Serial2.IrqHandler();
}

void SERCOM2_Handler()
{
  Serial3.IrqHandler();
}

void Set_Config1(void)
{
  String str1 = "";

  //*********************************************************************

  str1 = Send_Cmd("sip factory_reset");//reset
  Serial.print("sip factory_reset");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("sip reset");//reset
  Serial.print("sip reset");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 0 923200000");//0
  Serial.print("set_ch_freq 0");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 1 923400000");//1
  Serial.print("set_ch_freq 1");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 2 922000000");//2
  Serial.print("set_ch_freq 2");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 3 922200000");//3
  Serial.print("set_ch_freq 3");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 4 922400000");//4
  Serial.print("set_ch_freq 4");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 5 922600000");//5
  Serial.print("set_ch_freq 5");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 6 922800000");//6
  Serial.print("set_ch_freq 6");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_ch_freq 7 923000000");//7
  Serial.print("set_ch_freq 7");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_rx2 2 923200000");//8
  Serial.print("set_rx2 2");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac save");//8
  Serial.print("mac save");
  Serial.println(str1);
}

/********************************************************
  Function Name   : Serial2_String_Write
  Description     :
  Input           :
  Return          :
********************************************************/
void Serial2_String_Write(char *s)
{
  while (*s)
  {
    Serial2.write(*s++);
  }
}

/********************************************************
  Function Name   : Send_Cmd
  Description     :
  Input           :
  Return          :
********************************************************/
String Send_Cmd(char *s)
{
  int count = 0;
  Serial2_String_Write(s);
  while (Serial2.available() <= 0) {
    if (count >= 5000) {
      return "send cmd timeout\r\n";
    }
    count = count + 1;
    delay(1);
  }
  String str = Serial2.readString();
  return str;
}

/********************************************************
  Function Name   : Set_ConfigABP_c
  Description     :
  Input           :
  Return          :
********************************************************/

void Set_ConfigABP_A(void)
{
  String str1 = "";
  //*********************************************************************
  str1 = Send_Cmd("mac set_class A");
  Serial.print("set_class A");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_deveui xxxxx");
  Serial.print("set_deveui");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_devaddr xxxxx");
  Serial.print("set_devaddr");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_nwkskey xxxxx");
  Serial.print("set_nwkskey");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac set_appskey xxxxx");
  Serial.print("set_appskey");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac save");
  Serial.print("mac save");
  Serial.println(str1);

  //*********************************************************************
  str1 = Send_Cmd("mac join abp");
  Serial.print("mac join abp");
  Serial.println(str1);

}

unsigned long startMillis;
unsigned long currentMillis;
const unsigned long period = 15000;

/********************************************************
  Function Name   : setup
  Description     :
  Input           :
  Return          :
********************************************************/
void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(115200);
  Serial3.begin(9600);

  pinMode(12, OUTPUT);

  pinPeripheral(10, PIO_SERCOM);
  pinPeripheral(11, PIO_SERCOM);

  pinPeripheral(5, PIO_SERCOM);
  pinPeripheral(22, PIO_SERCOM);

  Serial.println("START");
  delay(5000);
  if (Serial2.available())
  {
    Serial.print(Serial2.readString());
  }
  delay(1000);
  Set_Config1();
  Set_ConfigABP_A();

  delay(250);
  Wire.begin();

  String str1 = "";
  str1 = Serial2.readString();
  Serial.println(str1);
  startMillis = millis();
}

void loop() {
  //=============================================================================
  String packet = "", data = "", msg  , str4 = "", pm1_0, pm2_5, pm10_0;

  if (Serial1.available() && pms.read(pmdata)) {     // If anything comes in Serial1 (pins 0 & 1)
    packet = Serial1.readString();
    msg = "mac tx ucnf 15 ";
    data = packet + String(pmdata.PM_AE_UG_1_0) + "D1_0" + String(pmdata.PM_AE_UG_2_5) + "D2_5" + String(pmdata.PM_AE_UG_10_0) + "D10_0";
    Serial.println(data);
    for (int i = 0; i < data.length(); i++) {
      msg += String(int(data.charAt(i)), HEX);
    }


    Serial.println(msg);
    char buf[msg.length() + 1];
    msg.toCharArray(buf, msg.length() + 1);

  //}


  currentMillis = millis();
  // SEND INTERVAL 15 SECOND --> EDIT PARAMETER preiod TO CHANGE TIME INTERVAL LINE 185

  if (currentMillis - startMillis >= period)
    {
    // SEND UPLINK DATA
    Serial2_String_Write(buf);
    startMillis = currentMillis;
    }
  // COUNT FOR SEND UPLINK
  if ((currentMillis - startMillis) / 1000 > 0)
    str4 = Serial2.readString();
  Serial.println(str4);

  }
}
