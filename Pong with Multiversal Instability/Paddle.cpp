struct Paddle {
    float x1,y1, x2,y2;
};

void paddleDraw(const struct Paddle& paddle) {
    glRectf(paddle.x1, paddle.y1, paddle.x2, paddle.y2);
}

void paddleCenterY(struct Paddle& paddle, float y)
{
    paddle.y1 = y + 50.0;
    paddle.y2 = y - 50.0;
}

void paddleLeftX(struct Paddle& paddle, float x) {
    paddle.x1 = x;
    paddle.x2 = paddle.x1 + 10.0;
}

void paddleRightX(struct Paddle& paddle, float x) {
    paddle.x2 = x;
    paddle.x1 = paddle.x2 - 10.0;
}

void paddleMoveY(struct Paddle& paddle, float paddleSpeed) {
    paddle.y1 += paddleSpeed;
    paddle.y2 += paddleSpeed;
}

bool paddleContains(const struct Paddle& paddle, float x, float y) {
    return x <= paddle.x2 && x >= paddle.x1 && y >= paddle.y2 && y <= paddle.y1;
}
