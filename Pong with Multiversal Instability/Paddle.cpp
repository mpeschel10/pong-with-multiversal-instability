struct Point {
    float x, y;
};

struct Point operator*(float c, struct Point p) { return Point{p.x * c, p.y * c}; }
struct Point operator*(struct Point p, float c) { return Point{p.x * c, p.y * c}; }
struct Point operator+(struct Point p1, struct Point p2) { return Point{p1.x + p2.x, p1.y + p2.y}; }

float pointDistance(const struct Point& p1, const struct Point& p2) {
    float dx = p1.x - p2.x;
    float dy = p1.y - p2.y;
    return sqrt(dx * dx + dy * dy);
}

void pointDraw(const struct Point& point) {
    glBegin(GL_POINTS);
    glVertex2f(point.x, point.y);
    glEnd();
}

struct Bezier1 { struct Point p0, p1; };
struct Bezier2 { struct Point p0, p1, p2; };

struct Point bezierInterpolate(const struct Bezier1& path, float t) {
    return (1-t) * path.p0 + t * path.p1;
}

struct Point bezierInterpolate(const struct Bezier2& path, float t) {
    return (1-t) * bezierInterpolate(Bezier1 {path.p0, path.p1}, t) +
               t * bezierInterpolate(Bezier1 {path.p1, path.p2}, t);
}

float bezierLength(const struct Bezier2& path, float startT, float endT) {
    float stepSize = (endT - startT) / 20;
    float spanLength = 0;

    struct Point previous = bezierInterpolate(path, startT);
    for (float t = startT + stepSize; t <= endT; t += stepSize) {
        struct Point next = bezierInterpolate(path, t);
        spanLength += pointDistance(previous, next);
        previous = next;
    }
    struct Point next = bezierInterpolate(path, endT);
    spanLength += pointDistance(previous, next);
    return spanLength;
}

// Approximates the difference in t necessary to move speed units.
// e.g. you say you want a step size of 10 true units; this says, "try t += 0.05"
float bezierSpeedToT(const struct Bezier2& path, float speed) {
    float lengthPerT = bezierLength(path, 0, 1);
    return speed / lengthPerT;
}

void bezierDraw(const struct Bezier2& path) {
    pointDraw(path.p0);
    pointDraw(path.p1);
    pointDraw(path.p2);
    
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0; t < 1.0; t += 0.05) {
        struct Point p = bezierInterpolate(path, t);
        glVertex2f(p.x, p.y);
    }
    struct Point p = bezierInterpolate(path, 1.0);
    glVertex2f(p.x, p.y);
    glEnd();
}


struct Paddle {
    float x1,y1, x2,y2;
    struct Bezier2 path;
    float targetSpeed, tOffset;
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
    std::cout << paddle.tOffset << std::endl;
}
