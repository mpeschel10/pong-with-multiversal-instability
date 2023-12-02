struct Paddle {
    float x1,y1, x2,y2;

    struct Bezier2 path;
    float targetSpeed, tOffset;
    float t;
};

void paddleDraw(const struct Paddle& paddle) {
    glRectf(paddle.x1, paddle.y1, paddle.x2, paddle.y2);
}

void paddleDraw(const struct Paddle& paddle, TexturedRectangle *texture) {
    texture->xywh(paddle.x1, paddle.y1, paddle.x2 - paddle.x1, paddle.y2 - paddle.y1);
    texture->display();
    // glRectf(paddle.x1, paddle.y1, paddle.x2, paddle.y2);
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

void paddleCenterX(struct Paddle& paddle, float x) {
    paddle.x1 = x - 5.0;
    paddle.x2 = x + 5.0;
}

void paddleMoveY(struct Paddle& paddle, float paddleSpeed) {
    paddle.y1 += paddleSpeed;
    paddle.y2 += paddleSpeed;
}

void paddleCenterPoint(struct Paddle& paddle, const struct Point& point) {
    paddleCenterY(paddle, point.y);
    paddleCenterX(paddle, point.x);
}

void paddleUpdate(struct Paddle& paddle) {
    paddleCenterPoint(paddle, bezierInterpolate(paddle.path, paddle.t));
}

void paddleMoveT(struct Paddle& paddle, float tOffset) {
    paddle.t += tOffset;
    paddleUpdate(paddle);
}

bool paddleContains(const struct Paddle& paddle, float x, float y) {
    return x <= paddle.x2 && x >= paddle.x1 && y >= paddle.y2 && y <= paddle.y1;
}

void paddleUpdateTOffset(struct Paddle& paddle) {
    paddle.tOffset = bezierSpeedToT(paddle.path, paddle.targetSpeed);
}

void paddleVerticalPath(struct Paddle& paddle, float x, float y0, float y2) {
    float y1 = (y2 + y0) / 2;
    paddle.path = Bezier2 {
        Point { x, y0 },
        Point { x, y1 },
        Point { x, y2 },
    };
    paddleUpdateTOffset(paddle);
}
