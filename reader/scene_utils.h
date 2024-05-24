#pragma once

#include "../geom/geometry.h"
#include "material.h"
#include "object.h"
#include "light.h"

#include <string_view>
#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>


std::vector<std::string> Split(const std::string& str, char delimiter){
    std::vector<std::string> splitted;

    if (str.empty())
        return splitted;
    
    size_t begin = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos){
        splitted.push_back(str.substr(begin, end - begin));
        begin = end + 1;
        end = str.find(delimiter, begin);
    }
    splitted.push_back(str.substr(begin));
    return (splitted);
}

void ChangeToSpace(std::string* line) {
    for (size_t ind = 0; ind < line->size(); ++ind) {
        if (isspace(line->at(ind)) && line->at(ind) != ' ') {
            line->at(ind) = ' ';
        }
    }
}

size_t FindNonEmpty(std::vector<std::string> splitted, int begin) {
    for (size_t ind = begin + 1; ind < splitted.size(); ++ind) {
        if (!splitted[ind].empty()) {
            return ind;
        }
    }
    return 0;
}

std::array<double, 3> RetreiveVector(const std::vector<std::string>& splitted, size_t* begin) {
    std::array<double, 3> vec;
    size_t vec_indx = 0;
    size_t splt_indx = *begin;

    while (vec_indx < vec.size()) {
        splt_indx = FindNonEmpty(splitted, splt_indx);
        if (!(splt_indx < splitted.size())) {
            std::cerr << "Error: no requited argument!\n";
            return vec;
        }
        vec[vec_indx++] = std::stod(splitted[splt_indx]);
    }
    *begin = splt_indx;
    return vec;
}

Sphere RetreiveSphere( std::vector<std::string>& splitted, size_t* begin) {
    std::array<double, 3> center;
    double radius = 0.0;
    size_t splt_indx = *begin;

    center = RetreiveVector(splitted, &splt_indx);
    splt_indx = FindNonEmpty(splitted, splt_indx);
    radius = std::stod(splitted[splt_indx]);
    return Sphere(center, radius);
}

Light RetreiveLight(const std::vector<std::string>& splitted, size_t* begin) {
    std::array<double, 3> position;
    std::array<double, 3> intensity;

    size_t splt_indx = *begin;

    position = RetreiveVector(splitted, &splt_indx);
    intensity = RetreiveVector(splitted, &splt_indx);
    *begin = splt_indx;
    return Light(position, intensity);
}

std::array<int, 3> GetIndex(std::string str_vrtx_indx, std::array<size_t, 3> sizes) {
    std::vector<std::string> splitted_by_bar = Split(str_vrtx_indx, '/');
    std::array<int, 3> indexes{0,0,0};

    for (size_t ind = 0; ind < indexes.size() && ind < splitted_by_bar.size(); ++ind) {
        if (!splitted_by_bar[ind].empty()) {
            indexes[ind] = std::stoi(splitted_by_bar[ind]);
            if (indexes[ind] < 0) {
                indexes[ind] = sizes[ind] + indexes[ind];
            }
        }
    }
    return indexes;
}

void FillMaterial(std::vector<std::string> splitted, Material* material, size_t* begin) {
    size_t ind = *begin;

    if (splitted[ind] == "Ka") {
        material->ambient_color = RetreiveVector(splitted, &ind);
    } else if (splitted[ind] == "Kd") {
        material->diffuse_color = RetreiveVector(splitted, &ind);
    } else if (splitted[ind] == "Ks") {
        material->specular_color = RetreiveVector(splitted, &ind);
    } else if (splitted[ind] == "Ke") {
        material->intensity = RetreiveVector(splitted, &ind);
    } else if (splitted[ind] == "Ns") {
        material->specular_exponent = std::stod(splitted[FindNonEmpty(splitted, ind)]);
    } else if (splitted[ind] == "Ni") {
        material->refraction_index = std::stod(splitted[FindNonEmpty(splitted, ind)]);
    } else if (splitted[ind] == "al") {
        material->albedo = RetreiveVector(splitted, &ind);
    }
    *begin = ind;
}