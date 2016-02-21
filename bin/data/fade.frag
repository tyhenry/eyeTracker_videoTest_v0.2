// #ifdef GL_ES
// precision mediump float;
// #endif
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

    vec4 color = texC;

    //desaturate
    float desatPct = circle(st, u_mouse, 300., 150.);
    color.rgb = mix(desaturate(color.rgb),color.rgb,pow(desatPct,3.));

    //dim
    float dimPct = circle(st, u_mouse, 250., 70.);
    float dimPctSpot = circle(st, u_mouse, 150., 80.);
    float dark = map(dimPctSpot, .0,1., .7,1.) * map(dimPct, .0,1., .4,1.);

    //brighten

    float black = .1 ;
    float white = 1.15;
    float blackPct = circle(st, u_mouse, 200., 60.);
    float r = map(color.r, .0,1., black,white);
    float g = map(color.g, .0,1., black,white);
    float b = map(color.b, .0,1., black,white);
    color.rgb = mix(color.rgb,vec3(r,g,b),blackPct);


    gl_FragColor = vec4(color.rgb * dark,1.);


}





