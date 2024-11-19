#ifndef FOCUSER_BUTTONS_H
#define FOCUSER_BUTTONS_H
#include <OneButton.h>
#include <unordered_map>
#include <array>

class Buttons {
public:
    static Buttons &Instance;
    Buttons();
    void setup();
    void loop();
    enum Button { Up, Down, Left, Right, UpLeft, UpRight, DownLeft, DownRight, Center };
    enum Mode { Single, Double, Long };

    static void onClick(Button button, Mode mode);
private:
    struct ButtonInfo {
        Button type;
        uint8_t pin;
        OneButton oneButton;
        void setup();
        void tick() { oneButton.tick(); }
    };
    std::array<ButtonInfo, 9> buttons;
};

#endif

