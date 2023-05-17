#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory.h>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <cassert>
#include <math.h>

/**
* Функция, преобразующая строку в float
*/
float* InFloat(std::string Numbers, int n_numb)
{
    float* sp = new float[n_numb];
    std::istringstream ss(Numbers);
    for (int i = 0; i < n_numb; i++)
    {
        ss >> sp[i];
    }

    return sp;
}

/**
* Функция, открывающая .obj файл
*/
std::vector <Vector3> OpenObj(const std::string file_name)
{
    std::string line;
    std::vector <Vector3> v_points = {};
    Vector3 point = {};
    float* v;
    point.x = 10;
    point.y = 0;
    point.z = 0;

    std::ifstream in;
    in.open(file_name);

    while (getline(in, line))
    {
        if (line.substr(0, 2) == "v ")
        {
            v = InFloat(line.substr(2, -1), 3);
            point.x = v[0];
            point.y = v[1];
            point.z = v[2];
            v_points.push_back(point);
        }
    }
    in.close();
    return v_points;
}

/**
* Функция, открывающая .lidar файл
*/
std::vector <Vector3> OpenLidar(std::string file_name)
{
    std::string line;
    std::vector <Vector3> v_points = {};
    Vector3 point = {};
    float* v;
    point.x = 0;
    point.y = 0;
    point.z = 0;
    int dist = 0;
    float psi = 0, phi = 0;

    std::ifstream in;
    in.open(file_name);

    while (getline(in, line))
    {
        v = InFloat(line, 3);
        dist = v[0] / 100;
        psi = v[1];
        phi = v[2];
        point.x = dist * cos(psi) * cos(phi);
        point.y = dist * cos(psi) * sin(phi);
        point.z = dist * sin(psi);
        v_points.push_back(point);
    }
    in.close();

    return v_points;
}

/**
* Функция, вычисляющая минимальную координату по Y из всех точек
*/
float MinCoord(std::vector <Vector3> v)
{
    float m = v[0].z;
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].z < m) m = v[i].z;
    }
    return m;
}

/**
* Функция, вычисляющая максимальную координату по Y из всех точек
*/
float MaxCoord(std::vector <Vector3> v)
{
    float m = v[0].z;
    for (int i = 0; i < v.size(); i++)
    {
        if (v[i].z > m) m = v[i].z;
    }
    return m;
}

/**
* Функция, раскрашивающая точки в разный цвет в зависимости от координаты по Y
*/
Vector3 GradientColorY(float z, float z_max, float z_min)
{
    Vector3 xyz_color = {};


    float a_r = 8 / ((z_max - z_min) * (z_max - z_min) * 5);
    float b_r = -(13 * z_max + 3 * z_min) / (5 * (z_max - z_min) * (z_max - z_min));
    float c_r = z_max * (5 * z_max + 3 * z_min) / (5 * (z_max - z_min) * (z_max - z_min));

    float a_g = 8 / ((z_max - z_min) * (z_max - z_min) * 5);
    float b_g = (13 * z_max + 3 * z_min) / (5 * (z_max - z_min) * (z_max - z_min));
    float c_g = z_max * (5 * z_max + 3 * z_min) / (5 * (z_max - z_min) * (z_max - z_min));

    float a_b = -4 / ((z_max - z_min) * (z_max - z_min));
    float b_b = 4 * (z_max + z_min) / ((z_max - z_min) * (z_max - z_min));
    float c_b = -4 * (z_max * z_min) / ((z_max - z_min) * (z_max - z_min));

    xyz_color.x = 0;
    xyz_color.y = 0;
    xyz_color.z = 0;

    xyz_color.x = 255 * (z * z * a_r + b_r * z + c_r);
    xyz_color.y = 255 * (z * z * a_g + b_g * z + c_g);
    xyz_color.z = 255 * (z * z * a_b + b_b * z + c_b);

    return xyz_color;
}


