struct Point {
    float x, y;
};

struct Point operator*(float c, const struct Point& p) { return Point{p.x * c, p.y * c}; }
struct Point operator*(const struct Point& p, float c) { return Point{p.x * c, p.y * c}; }
struct Point operator+(const struct Point& p1, const struct Point& p2) { return Point{p1.x + p2.x, p1.y + p2.y}; }
struct Point operator+(const struct Point& p, float addend) { return Point{p.x + addend, p.y + addend}; }
struct Point operator-(const struct Point& p1, const struct Point& p2) { return Point{p1.x - p2.x, p1.y - p2.y}; }
struct Point operator/(const struct Point& p, float r) { return Point{p.x / r , p.y / r}; }
std::ostream& operator<<(std::ostream& os, const struct Point& p) {
    return os << "Point {" << p.x << ", " << p.y << "}";
}

float dot(const struct Point& p1, const struct Point& p2) { return p1.x * p2.x + p1.y * p2.y; }
float magnitude(const struct Point& p) { return sqrt(p.x * p.x + p.y * p.y); }
struct Point normalize(const struct Point& p) { return p / magnitude(p); }
struct Point randomIn(const struct Point& p) { return Point {randomFloat(p.x), randomFloat(p.y)}; }
struct Point pointAngleToPoint(float angle) { return Point {cos(angle), sin(angle)}; }
float pointDistance(const struct Point& p1, const struct Point& p2) { return magnitude(p1 - p2); }

bool approximates(const struct Point& p1, const struct Point& p2, float error) {
    return round(p1.x / error) == round(p2.x / error) && round(p1.y / error) == round(p2.y / error);
}

struct Point reflect(const struct Point& incident, const struct Point& normal) {
    float m = magnitude(incident);
    const struct Point normalized_incident = incident / m;
    struct Point reflected = 2 * dot(normalized_incident, normal) * normal - normalized_incident;
    return reflected * m;
}

void pointDraw(const struct Point& point) {
    glBegin(GL_POINTS);
    glVertex2f(point.x, point.y);
    glEnd();
}
