#ifndef XRMain_h
#define XRMain_h

#include <XRHardware.h>

using namespace XRHardware;

class View {
public:
  virtual void HandleInput(Hardware &hw) = 0;
  virtual void Render(Hardware &hw, int xo=0, int yo=0, int w=XR1_DISPLAY_WIDTH, int h=XR1_DISPLAY_HEIGHT) = 0;
};

class GridView : public View {
public:
  bool grid[4][4];
  GridView() {
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
  virtual void Render(Hardware &hw, int xo=0, int yo=0, int w=XR1_DISPLAY_WIDTH, int h=XR1_DISPLAY_HEIGHT) {
    int buttonSpacing = std::min(w,h)/4;
    int buttonWidth = buttonSpacing * 0.8;
    for(int x=0;x<4;x++) {
      for(int y=0;y<4;y++) {
        if(grid[x][y]) {
          hw.display.drawRBox(xo+x*buttonSpacing,yo+y*buttonSpacing,buttonWidth,buttonWidth,2);
        } else {
          hw.display.drawRFrame(xo+x*buttonSpacing,yo+y*buttonSpacing,buttonWidth,buttonWidth,2);
        }
      }
    }
  }
};

class KeyboardView : public View {
public:
  bool keyboard[13];
  KeyboardView() {
    for(int i=0;i<13;i++) keyboard[i]=false;
  }
  virtual void HandleInput(Hardware &hw) {
    for(int i=0; i<13; i++) keyboard[i] = hw.keyboard.GetKeyState(i) != XRHardware::Keyboard::KeyState::IDLE;
  }
  virtual void Render(Hardware &hw, int xo=0, int yo=0, int w=XR1_DISPLAY_WIDTH, int h=XR1_DISPLAY_HEIGHT) {
    int buttonSpacing = w/13;
    int buttonWidth = buttonSpacing * 0.8;
    for(int i=0;i<13;i++) {
      if(keyboard[i]) {
        hw.display.drawRBox(
          xo+i*buttonSpacing,
          yo,
          buttonWidth,
          h,
          2
        );
      } else {
        hw.display.drawRFrame(
          xo+i*buttonSpacing,
          yo,
          buttonWidth,
          h,
          2
        );
      }
    }
  }
};

// some globals that we'll pretend are part of a sequencer for now
int bpm = 120;
int pattern = 0;
int track = 0;

class MainView : public View {
public:
  vector<View*> subViews;
  int activeSubView = 0;
  MainView() {
    subViews.push_back((View*)(new GridView()));
    subViews.push_back((View*)(new KeyboardView()));
  }
  virtual void HandleInput(Hardware &hw) {
    int mainEncDelta = hw.encoders.deltas[XRHardware::Encoders::ENC_MAIN_ADDRESSS].delta;
    if(hw.buttons.buttonStates[XRHardware::Buttons::BTN_TEMPO_CLOCK] == KeyState::HOLD) {
      bpm += mainEncDelta;
    } else if(hw.buttons.buttonStates[XRHardware::Buttons::BTN_PATTERN_BANK] == KeyState::HOLD) {
      pattern += mainEncDelta;
    } else if(hw.buttons.buttonStates[XRHardware::Buttons::BTN_TRACK_LAYER] == KeyState::HOLD) {
      track += mainEncDelta;
    } else {
      activeSubView = std::max(0,std::min((int)(subViews.size()-1), activeSubView+mainEncDelta));
    }
    subViews[activeSubView]->HandleInput(hw);
  }
  virtual void Render(Hardware &hw, int xo=0, int yo=0, int w=XR1_DISPLAY_WIDTH, int h=XR1_DISPLAY_HEIGHT) {
    hw.display.setCursor(0,0);
    hw.display.printf("BPM: %d", bpm);
    hw.display.setCursor(0,10);
    hw.display.printf("PATTERN: %d", pattern);
    hw.display.setCursor(0,20);
    hw.display.printf("TRACK: %d", track);
    subViews[activeSubView]->Render(hw, 45, 0, 80);
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
    rootView = (View*)(new MainView());
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