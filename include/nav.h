#ifndef FOCUSER_NAV_H
#define FOCUSER_NAV_H

class Nav {
public:
    static Nav &Instance;
    enum PressMode { Single, Double, Long };

    void left(PressMode mode);
    void right(PressMode mode);
    void up(PressMode mode);
    void down(PressMode mode);
    void center(PressMode mode);
private:
};

#endif

