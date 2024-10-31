//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul IV - 04_02_Shader.frag |
// ======================================
// 
//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;
//

#version 330 core

//	Variabile de intrare (dinspre Shader.vert);
in vec4 ex_Color;

in vec2 tex_Coord; /// 3. Coordonata de texturare;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;	// Culoarea actualizata;

/// 2. Variabilele uniforme;
uniform int codCol;

/// 3. Variabile uniforme;
uniform sampler2D myTexture;

vec4 blue = vec4(0.0, 0.0, 1.0, 1.0);
vec4 red = vec4(1.0, 0.0, 0.0, 1.0);

void main(void)
{
   switch(codCol) 
   {
	case 0:
		out_Color=ex_Color; // Culoarea este cea asociata varfurilor;        
		break;
		
	case 1: // Brown
		out_Color = vec4(0.64, 0.16, 0.16, 0.0);
		break;

	case 2: // Green
		out_Color = vec4(0.0, 1.0, 0.0, 0.0);
		break;

	case 3: // Black
		out_Color = vec4(0.0, 0.0, 0.0, 0.0);
		break;

	case 4: // Cyan
		out_Color = vec4(0.0, 1.0, 1.0, 0.0);
		break;

	case 5: // Grey
		out_Color = vec4(0.5, 0.5, 0.5, 0.0);
		break;

	/// 3.
	case 6: /// 3.1. Amestecarea a doua culori cu functia mix;
		out_Color = mix(red, blue, 0.75);
		break;
	
	case 7: /// 3.2. Amestecarea texturii si a culorii;
		out_Color = mix(texture(myTexture, tex_Coord), ex_Color, 0.3);
		break;

	default:
		break;
   }
}