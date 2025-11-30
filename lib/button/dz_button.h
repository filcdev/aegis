#ifndef DZ_BUTTON_H
#define DZ_BUTTON_H

class DZButton
{
public:
  DZButton();
  void begin();
  void handle();
private:
  unsigned long lastDebounceTime = 0;
};

#endif