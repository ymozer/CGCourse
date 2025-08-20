out vec4 FragColor;

// Uniform variable for the square's color, set from the C++ application
uniform vec4 u_SquareColor;

void main()
{
    FragColor = u_SquareColor;
}