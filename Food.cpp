#include "Food.h"

Food::Food(Vector2 pos) 
    : position(pos)
    , consumed(false) {
}

void Food::Draw() const {
    if (!consumed) {
        DrawCircle(position.x, position.y, SIZE, YELLOW);
    }
}
