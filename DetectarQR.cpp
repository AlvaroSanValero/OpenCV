#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

// Función para detectar el cuadrado rojo en la imagen
vector<Point> detectarCuadradoRojo(Mat& imagen) {
    vector<Point> cuadrado;

    // Convertir la imagen a espacio de color HSV
    Mat hsv;
    cvtColor(imagen, hsv, COLOR_BGR2HSV);

    // Definir el rango de colores rojos en HSV
    Mat mascara1, mascara2;
    inRange(hsv, Scalar(0, 100, 100), Scalar(10, 255, 255), mascara1);
    inRange(hsv, Scalar(170, 100, 100), Scalar(180, 255, 255), mascara2);

    // Combinar las máscaras
    Mat mascaraRojo;
    bitwise_or(mascara1, mascara2, mascaraRojo);

    // Encontrar contornos en la máscara
    vector<vector<Point>> contornos;
    findContours(mascaraRojo.clone(), contornos, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // Filtrar contornos por su área y forma
    for (const auto& contorno : contornos) {
        double area = contourArea(contorno);
        if (area > 100) {
            vector<Point> approx;
            approxPolyDP(contorno, approx, 0.02 * arcLength(contorno, true), true);

            // Si el contorno es un cuadrado
            if (approx.size() == 4) {
                cuadrado = approx;
                break;
            }
        }
    }

    return cuadrado;
}

// Función para calcular la orientación del cuadrado
string calcularOrientacion(const vector<Point>& cuadrado) {
    // Suponiendo que el cuadrado siempre se detecta frontalmente
    return "frontal";
}

// Función para calcular el movimiento necesario
string calcularMovimiento(const vector<Point>& cuadrado, int imagenAncho) {
    // Calcular centro del cuadrado
    Moments M = moments(cuadrado);
    int cX = M.m10 / M.m00;
    
    // Calcular el tamaño del cuadrado
    double area = contourArea(cuadrado);

    // Determinar el movimiento necesario
    string movimiento;
    if (area < 200) {
        movimiento = "Avanzar hacia adelante";
    } else if (cX < imagenAncho / 2 - 20) {
        movimiento = "Avanzar hacia la izquierda";
    } else if (cX > imagenAncho / 2 + 20) {
        movimiento = "Avanzar hacia la derecha";
    } else {
        movimiento = "Ok, quieto";
    }

    return movimiento;
}

int main() {
    // Leer la imagen desde el disco
    Mat imagen = imread("imagen_oficina.jpg");
    if (imagen.empty()) {
        cerr << "No se pudo cargar la imagen." << endl;
        return 1;
    }

    // Detectar el cuadrado rojo en la imagen
    vector<Point> cuadrado = detectarCuadradoRojo(imagen);

    // Si se detecta un cuadrado
    if (!cuadrado.empty()) {
        // Calcular orientación del cuadrado
        string orientacion = calcularOrientacion(cuadrado);
        
        // Calcular movimiento necesario
        string movimiento = calcularMovimiento(cuadrado, imagen.cols);

        // Mostrar resultados
        cout << "Orientación del cuadrado: " << orientacion << endl;
        cout << "Movimiento necesario: " << movimiento << endl;
    } else {
        cout << "No se detectó ningún cuadrado rojo en la imagen." << endl;
    }

    return 0;
}
