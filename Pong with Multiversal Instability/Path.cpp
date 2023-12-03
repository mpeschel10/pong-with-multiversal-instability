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
        spanLength += distance(previous, next);
        previous = next;
    }
    struct Point next = bezierInterpolate(path, endT);
    spanLength += distance(previous, next);
    return spanLength;
}

// Approximates the difference in t necessary to move speed units.
// e.g. you say you want a step size of 10 true units; this says, "try t += 0.05"
float bezierSpeedToT(const struct Bezier2& path, float speed) {
    float lengthPerT = bezierLength(path, 0, 1);
    return speed / lengthPerT;
}

void bezierDraw(const struct Bezier2& path) {
    pointDisplay(path.p0);
    pointDisplay(path.p1);
    pointDisplay(path.p2);
    
    glBegin(GL_LINE_STRIP);
    for (float t = 0.0; t < 1.0; t += 0.05) {
        struct Point p = bezierInterpolate(path, t);
        glVertex2f(p.x, p.y);
    }
    struct Point p = bezierInterpolate(path, 1.0);
    glVertex2f(p.x, p.y);
    glEnd();
}
