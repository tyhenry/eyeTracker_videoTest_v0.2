// #ifdef GL_ES
// precision mediump float;
// #endif
#version 120

uniform sampler2DRect u_tex0;
uniform vec2 u_tex0Resolution;

uniform vec2 u_resolution;
uniform vec2 u_mouse;

uniform vec2 u_dir;
uniform int u_vFlip = 1;

float blurRadius = 3.0;


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

float c2p(vec3 c){ //convert color to pct
    return (c.r + c.g + c.b)/3.;
}

vec3 desaturate(vec3 c){ //convert color to pct
    return vec3(c2p(c));
}

float circle(vec2 st, vec2 center, float radius, float smoothing){

    float dist = distance(st,center); //generate distance field cone

    float circle = smoothstep(radius, radius+smoothing, dist); //slice that cone

    return clamp(1.0 - circle, 0.0, 1.0); //invert + clamp (is clamp needed?)

}


void main(){
	vec2 st = gl_FragCoord.xy/u_resolution;
    
    if (u_vFlip == 1){ st.y = 1. - st.y; } // invert texture y
    st = st * u_resolution;

    vec4 texC = texture2DRect(u_tex0, st);

    vec4 sum = vec4(0.0);

    //the amount to blur, i.e. how far off center to sample from 
    //1.0 -> blur by one pixel
    //2.0 -> blur by two pixels, etc.
    float blur = blurRadius; 

    // horizontal OR vertical blur, never both
    float horz = u_dir.x;
    float vert = u_dir.y;

    //apply blurring, using a 9-tap filter with predefined gaussian weights

    sum += texture2DRect(u_tex0, vec2(st.x - 4.0*blur*horz, st.y - 4.0*blur*vert)) * 0.0162162162;
    sum += texture2DRect(u_tex0, vec2(st.x - 3.0*blur*horz, st.y - 3.0*blur*vert)) * 0.0540540541;
    sum += texture2DRect(u_tex0, vec2(st.x - 2.0*blur*horz, st.y - 2.0*blur*vert)) * 0.1216216216;
    sum += texture2DRect(u_tex0, vec2(st.x - 1.0*blur*horz, st.y - 1.0*blur*vert)) * 0.1945945946;

    sum += texture2DRect(u_tex0, vec2(st.x, st.y)) * 0.2270270270;

    sum += texture2DRect(u_tex0, vec2(st.x + 1.0*blur*horz, st.y + 1.0*blur*vert)) * 0.1945945946;
    sum += texture2DRect(u_tex0, vec2(st.x + 2.0*blur*horz, st.y + 2.0*blur*vert)) * 0.1216216216;
    sum += texture2DRect(u_tex0, vec2(st.x + 3.0*blur*horz, st.y + 3.0*blur*vert)) * 0.0540540541;
    sum += texture2DRect(u_tex0, vec2(st.x + 4.0*blur*horz, st.y + 4.0*blur*vert)) * 0.0162162162;

    //blur
    vec4 blurC = vec4(sum.rgb,texC.a);

    float fadeBlur = circle(st, u_mouse, 200., 100.);

    vec4 color = fadeBlur * texC + (1. - fadeBlur) * blurC;

    /*desaturate
    float fadePct = circle(norm(st), mouse, 0.15, 1.);
    color.rgb = mix(desaturate(color.rgb),color.rgb,pow(fadePct,3.));

    //dim
    float dark = map(fadePct, .0,1., .3,1.);
    */

    gl_FragColor = color;


}





