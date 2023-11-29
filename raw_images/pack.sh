DIR="../Pong with Multiversal Instability/textures"

# Ball textures
inkscape bg.svg -o "${DIR}/bg.png"
inkscape Baseball.svg -w 64 -h 64 -o "${DIR}/baseball.png"
inkscape Basketball_Clipart.svg -w 64 -h 64 -o "${DIR}/basketball.png"
inkscape Orange_fruit_icon.svg -w 64 -h 64 -o "${DIR}/orange.png"
cp ping-pong_small.png "${DIR}/ping-pong.png"

# Paddle textures
inkscape barber_pole.svg -b "#ffffff" -w 10 -h 100 -o "${DIR}/barber.png"
inkscape Table-tennis.svg -w 10 -h 100 -o "${DIR}/paddle.png"
inkscape French_fries_juliane_kr_r.svg -w 10 -h 100 -o "${DIR}/fries.png"
inkscape Sword_2rte.svg -w 10 -h 100 -o "${DIR}/sword.png"
