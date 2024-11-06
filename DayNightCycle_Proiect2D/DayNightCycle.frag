//  Shaderul de fragment / Fragment shader - afecteaza culoarea pixelilor;

#version 330	//  Versiunea GLSL;

//	Variabile de intrare (dinspre Shader.vert);
in vec4 ex_Color;

//	Variabile de iesire	(spre programul principal);
out vec4 out_Color;		//	Culoarea actualizata;

//	Variabilele uniforme;
uniform int codCol;

//	Actualizeaza culoarea in functie de codCol;
void main(void)
{
  switch (codCol)
  {
	case 0: // GLfloat Colors[] = {....};
	  out_Color = ex_Color;
	  break;

	case -1: // Black
		out_Color = vec4(0., 0., 0., 0.);
		break;

	case -2: // White
		out_Color = vec4(1., 1., 1., 0.);
		break;

	case 1: // Brown
		out_Color = vec4(0.64, 0.16, 0.16, 0.);
		break;

	case 2: // Green
		out_Color = vec4(0., 1., 0., 0.);
		break;

	case 3: // Cyan
		out_Color = vec4(0., 1., 1., 0.);
		break;

	case 4: // Grey
		out_Color = vec4(0.5, 0.5, 0.5, 0.);
		break;

	case 5: // Bright Green
		out_Color = vec4(0.49f, 0.99f, 0.f, 1.f);
		break;

	case 6: // Dark Green
		out_Color = vec4(0., 0.5f, 0.f, 1.f);
		break;

	case 7: // Water Color
		out_Color = vec4(0., 0.5f, 1.f, 0.7f);
		break;

	default:
		break;
  };
}