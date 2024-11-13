#ifndef XRMain_h
#define XRMain_h

#include <XRHardware.h>

using namespace XRHardware;

class View
{
public:
  virtual void HandleInput(Hardware &hw) = 0;
  virtual void Render(Hardware &hw) = 0;
};

class DefaultView : public View {
public:
  bool grid[4][4];
  DefaultView() {
    for(int x=0;x<4;x++) {
      for(int y=0;y<4;y++) {
        grid[x][y] = false;
      }
    }
  }
  virtual void HandleInput(Hardware &hw) {
    for(int x=0;x<4;x++) {
      for(int y=0;y<4;y++) {
        if(hw.buttons.buttonStates[x+y*4] == KeyState::PRESSED) {
          grid[x][y] = !grid[x][y];
          hw.leds.setPWM(x + y*4, grid[x][y] ? 4095 : 0);
        }
      }
    }
  }
  virtual void Render(Hardware &hw) {
    for(int x=0;x<4;x++) {
      for(int y=0;y<4;y++) {
        if(grid[x][y]) {
          hw.display.drawRBox(1+x*14,1+y*14,12,12,2);
        } else {
          hw.display.drawRFrame(1+x*14,1+y*14,12,12,2);
        }
      }
    }
  }
};

Hardware hw;
IntervalTimer controlTimer;
View* rootView;

namespace XRMain
{
  void handleControls() {
    hw.UpdateControls();
    rootView->HandleInput(hw);
  }

  void boot()
  {
    hw.Init();
    rootView = (View*)(new DefaultView());
    controlTimer.begin(handleControls, 1000000/(96*2));
  }

  void update()
  {
    hw.display.clearBuffer();
    rootView->Render(hw);
    hw.display.sendBuffer();
  }
}

#endif /* XRMain_h */