#define BLINKER_WIFI
#define BLINKER_ALIGENIE_SENSOR   // tj - 定义为天猫精灵传感器设备
#include <Blinker.h>
#include <DHT.h>
#define tmin 10   //tj-设置温度阈值
#define hmin 50   //tj-设置湿度阈值

char auth[] = "****";  // tj-申请到的key
char ssid[] = "****";  // tj-wifi名称
char pswd[] = "****";  // tj-wifi密码

BlinkerNumber HUMI("num-shidu");
BlinkerNumber TEMP("num-wendu");


#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11

DHT dht(DHTPIN, DHTTYPE);

uint32_t read_time = 0;

float humi_read, temp_read;

void dataRead(const String & data)
{
  BLINKER_LOG("Blinker readString: ", data);

  Blinker.vibrate();

  uint32_t BlinkerTime = millis();

  Blinker.print("millis", BlinkerTime);
}

void heartbeat()
{
  HUMI.print(humi_read);
  TEMP.print(temp_read);
}


void setup()
{
  Serial.begin(115200);
  BLINKER_DEBUG.stream(Serial);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Blinker.begin(auth, ssid, pswd);
  Blinker.attachData(dataRead);
  Blinker.attachHeartbeat(heartbeat);
  BlinkerAliGenie.attachQuery(aligenieQuery);
  dht.begin();
}
void aligenieQuery(int32_t queryCode)
{
    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("AliGenie Query All");
            BlinkerAliGenie.temp(temp_read);
            BlinkerAliGenie.humi(humi_read);
            BlinkerAliGenie.print();
            break;
        case BLINKER_CMD_QUERY_TEMP_NUMBER :
            BlinkerAliGenie.temp(temp_read);
            BlinkerAliGenie.print();
            break;
        case BLINKER_CMD_QUERY_HUMI_NUMBER :
            BlinkerAliGenie.humi(humi_read);
            BlinkerAliGenie.print();
            break;
        default :
            BlinkerAliGenie.temp(20);
            BlinkerAliGenie.humi(20);
            BlinkerAliGenie.print();
            break;
    }
}
void loop()
{
  Blinker.run();

  if (read_time == 0 || (millis() - read_time) >= 2000)
  {
    read_time = millis();

    float h = dht.readHumidity();
    float t = dht.readTemperature();

    if (isnan(h) || isnan(t)) {
      BLINKER_LOG("Failed to read from DHT sensor!");
      return;
    }

    float hic = dht.computeHeatIndex(t, h, false);

    humi_read = h;
    temp_read = t;

    BLINKER_LOG("Humidity: ", h, " %");
    BLINKER_LOG("Temperature: ", t, " *C");
    BLINKER_LOG("Heat index: ", hic, " *C");

    String wd, sd, s1, s2, s3 , s4, wdsd ;
    s1 = t;
    s2 = " Warning!The tempreture is :";
    s3 = h;
    s4 = " Warning!The humidity is :";
    wd = s2 + s1;
    sd = s4 + s3;
    wdsd = s2 + s1 + s4 + s3 ;
    if (t >= tmin && h >= hmin) Blinker.wechat(wdsd) ;  //tj-情况一：温度、湿度均高于阈值
    else if (h >= hmin) Blinker.wechat(sd) ;            //tj-情况二：仅湿度高于阈值
    else if (t >= tmin) Blinker.wechat(wd);         //tj-情况三：仅温度高于阈值
  }
}
