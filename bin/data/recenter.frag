#version 120

uniform sampler2DRect u_tex0;
uniform vec2 u_tex0Resolution;

uniform vec2 u_resolution;
uniform vec2 u_mouse;
uniform int u_vFlip = 1;

//custom map function (boolean clamps)
float map(float val, float lo, float hi, float newLo, float newHi, bool bClamp){

    //clamp
    if (bClamp) {
        if (val < lo){ return newLo; }
        if (val > hi){ return newHi; }
    }

    float pct = (val-lo)/(hi-lo); //normalize val
    return ((newHi-newLo) * pct) + newLo; //calc + shift normalized val
}

//custom map function (auto-clamps)
float map(float val, float lo, float hi, float newLo, float newHi){

    //clamp
    if (val < lo){ return newLo; }
    if (val > hi){ return newHi; }

    float pct = (val-lo)/(hi-lo); //normalize val
    return ((newHi-newLo) * pct) + newLo; //calc + shift normalized val
}

vec2 norm(vec2 xy){
	return xy/u_resolution;
}


void main(){
	vec2 st = gl_FragCoord.xy/u_resolution;
    if (u_vFlip == 1){ st.y = 1. - st.y; } // invert texture y
    //recenter st
    st += norm(u_mouse) - 0.5;
    st = st * u_resolution;

    vec4 color = texture2DRect(u_tex0, st);
    if (st.x > u_resolution.x || st.x < 0 || st.y > u_resolution.y || st.y < 0){
        color.a = 0;
    }

    gl_FragColor = color;


}





