uniform sampler2D texture;
uniform float blur_radius_x;
uniform float blur_radius_y;

void main()
{
	vec2 offx = vec2(blur_radius_x, 0.0);
	vec2 offy = vec2(0.0, blur_radius_y);
	vec4 multcolor = vec4(0.55, 0.58, 0.65, 1.0);

	// get texture color
	gl_FragColor  = gl_Color;
	
	// blur
	gl_FragColor += texture2D(texture, gl_TexCoord[0].xy)               * 4.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy - offx)        * 2.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy + offx)        * 2.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy - offy)        * 2.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy + offy)        * 2.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy - offx - offy) * 1.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy - offx + offy) * 1.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy + offx - offy) * 1.0;
    gl_FragColor += texture2D(texture, gl_TexCoord[0].xy + offx + offy) * 1.0;
	gl_FragColor /= 16.0;
	
	// desaturate
	gl_FragColor.r = gl_FragColor.r * 0.4f +  gl_FragColor.g * 0.3f +  gl_FragColor.b * 0.3f;
	gl_FragColor.g = gl_FragColor.r * 0.3f +  gl_FragColor.g * 0.4f +  gl_FragColor.b * 0.3f;
	gl_FragColor.b = gl_FragColor.r * 0.3f +  gl_FragColor.g * 0.3f +  gl_FragColor.b * 0.4f;
	
	// multiply for color tone
	gl_FragColor *= multcolor;
}
