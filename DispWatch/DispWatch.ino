#include <Nefry.h>
#include <NefryDisplay.h>
#include <NefrySetting.h>
void setting() {
  Nefry.disableDisplayStatus();
  Nefry.disableWifi();
}
NefrySetting nefrySetting(setting);

//date
#include <time.h>
#define JST     3600*9

//ループ周期(us)
#include "interval.h"
#define LOOPTIME 10000

//clock
#define CL_HOUR 0
#define CL_MIN 1
#define CL_SEC 2
//左上の座標と円サイズ
#define CL_X 32
#define CL_Y 32
#define CL_R 30

uint8_t Hour = 0;
uint8_t Min = 0;
uint8_t Sec = 0;



//グラフ一時停止
bool isStopGraph = false;

#include "dispGraphCircle.h"
graph_circle grcir = graph_circle();

void setup() {
  Nefry.enableSW();

  NefryDisplay.begin();
  NefryDisplay.setAutoScrollFlg(true);//自動スクロールを有効

  NefryDisplay.clear();
  NefryDisplay.display();

  dispGraphCircle_init();

  //date
  configTime( JST, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp");

}

void dispGraphCircle_init() {
  //グラフ番号・頂点座標(x,y)・半径(r)・対象の値(最小値,最大値)・塗りつぶし有り無し
  grcir.setGraph(CL_HOUR, CL_X, CL_Y, 20, 0, 60, false);
  grcir.setGraph(CL_MIN, CL_X, CL_Y, 25, 0, 60, false);
  grcir.setGraph(CL_SEC, CL_X, CL_Y, 10, 0, 60, true);
}

void DispNefryDisplay() {
  NefryDisplay.clear();
  //外枠
  NefryDisplay.drawCircle(CL_X, CL_Y, CL_R);

  //時計の針の描画
  grcir.updateGraph();

  NefryDisplay.setFont(ArialMT_Plain_24);
  String _tmpH = String(Hour);
  if (Hour < 10) _tmpH = '0' + String(Hour);
  String _tmpM = String(Min);
  if (Min < 10) _tmpM = '0' + String(Min);
  String _tmpS = String(Sec);
  if (Sec < 10) _tmpS = '0' + String(Sec);

  NefryDisplay.drawString(70, 0, _tmpH);
  NefryDisplay.drawString(70, 21, _tmpM);
  NefryDisplay.drawString(70, 42, _tmpS);

  NefryDisplay.display();
}


void loop() {
  //グラフ更新一時停止
  if (Nefry.readSW()) {
    isStopGraph = !isStopGraph;
  }

  //データ・グラフ更新
  interval<LOOPTIME>::run([] {
    if (!isStopGraph) {
#if false
      //日付を取得する
      time_t  t = time(NULL);
      struct tm *tm;
      tm = localtime(&t);
      Hour = tm->tm_hour;
      Min = tm->tm_min;
      Sec = tm->tm_sec;
#endif

      //debug
      if (++Sec >= 60) {
        Sec = 0;
        if (++Min >= 60) {
          Min = 0;
          if (++Hour >= 24) {
            Hour = 0;
          }
        }
      }

      //24h -> 12h表記にする
      uint8_t _tmpHour = Hour % 12;
      //分に合わせて時間の針を進める
      _tmpHour = _tmpHour * 5 + Min / 12;

      grcir.addGraphData(CL_HOUR, (int)_tmpHour);
      grcir.addGraphData(CL_MIN, (int)Min);
      grcir.addGraphData(CL_SEC, (int)Sec);
    }

    DispNefryDisplay();
  });
}
