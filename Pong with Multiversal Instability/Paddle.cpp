struct Paddle {
    float x1,y1, x2,y2;
};

void paddleDraw(const struct Paddle& paddle) {
    glRectf(paddle.x1, paddle.y1, paddle.x2, paddle.y2);
}
