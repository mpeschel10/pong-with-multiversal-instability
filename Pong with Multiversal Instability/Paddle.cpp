struct Point {
    float x, y;
};

struct Point operator*(float c, struct Point p) { return Point{p.x * c, p.y * c}; }
struct Point operator*(struct Point p, float c) { return Point{p.x * c, p.y * c}; }
struct Point operator+(struct Point p1, struct Point p2) { return Point{p1.x + p2.x, p1.y + p2.y}; }

void pointDraw(const struct Point& point) {
    glBegin(GL_POINTS);
    glVertex2f(point.x, point.y);
    glEnd();
}

struct Bezier1 { struct Point p0, p1; };
struct Bezier2 { struct Point p0, p1, p2; };

struct Point bezierInterpolate(float t, const struct Bezier1& path) {
    return (1-t) * path.p0 + t * path.p1;
}

struct Point bezierInterpolate(float t, const struct Bezier2& path) {
    return (1-t) * bezierInterpolate(t, Bezier1 {path.p0, path.p1}) +
               t * bezierInterpolate(t, Bezier1 {path.p1, path.p2});
}

void bezierDraw(const struct Bezier2& path) {
    pointDraw(path.p0);
    pointDraw(path.p1);
    pointDraw(path.p2);
    
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0; t < 1.0; t += 0.05) {
        struct Point p = bezierInterpolate(t, path);
        glVertex2f(p.x, p.y);
    }
    struct Point p = bezierInterpolate(1.0, path);
    glVertex2f(p.x, p.y);
    glEnd();
}


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
