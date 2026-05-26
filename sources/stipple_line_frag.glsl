#version 410

uniform float stipplePattern[16];
// picking
uniform int objName; // ピッキング時以外は 0 を想定

in highp float gStippleCoord;
in highp vec4 gColor;

layout (location = 0) out vec4 fc; 

void main() {
    // 点線（スティップル）の隙間を破棄する処理（元のロジックを維持）
    float stip = stipplePattern[int(fract(gStippleCoord) * 16.0)];
    if (stip == 0.0)
        discard;

    if (objName != 0) {
        // objName (int) を 24bit整数として RGBA の各チャンネルにエンコード
        int r = objName & 0xFF;
        int g = (objName >> 8) & 0xFF;
        int b = (objName >> 16) & 0xFF;
        
        fc = vec4(float(r) / 255.0, float(g) / 255.0, float(b) / 255.0, 1.0);
    } else {
        // 通常描画モード
        fc = gColor;
    }
}