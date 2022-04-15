////////////////////////////////////////////////////////////////////////////////
/*
 * File: instance.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief CVRP instance class definition.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 18, 2020, 01:36 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <cmath>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/instance.hpp"

/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

/**
 * @brief Computes the euclidian distance between every pair of vertices of the
 * coordinates vector.
 * @param:
*/
std::vector<std::vector<double>> computeDistances(
    const std::vector<std::pair<double, double>>& coord)
{
    std::vector<std::vector<double>> cij(coord.size(),
                                         std::vector<double>(coord.size(), 0));

    for (std::size_t i = 0; i < coord.size(); ++i)
    {
        for (std::size_t j = i + 1; j < coord.size(); ++j)
        {
            auto tmp = std::sqrt(std::pow(coord[i].first - coord[j].first, 2) +
                                 std::pow(coord[i].second - coord[j].second,2));
            cij[i][j] = static_cast<int>(std::round(tmp));
            cij[j][i] = cij[i][j];
        }
    }

    return cij;
}

/**
 * @brief.
*/
std::vector<std::pair<double, double>> readCoordinates(
    std::ifstream& file,
    const int nbVertices)
{
    std::vector<std::pair<double, double>> coord;
    coord.reserve(nbVertices);

    int i = 0;
    while (i < nbVertices)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double x, y;
        iss >> x >> x >> y; // skip index
        coord.push_back(std::make_pair(x, y));
        ++i;
    }

    return coord;
}

/**
 * @brief.
*/
std::vector<std::vector<double>> readCostMtx(
    std::ifstream& file,
    const int nbVertices)
{
    std::vector<std::vector<double>> cij(nbVertices,
                                         std::vector<double>(nbVertices, 0));

    int nbRows = std::ceil((std::pow(nbVertices, 2) - nbVertices) / 20);
    int c = 0, i = 1, j = 0;
    while (c < nbRows)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double w;
        while (iss >> w)
        {
            if (j >= i)
            {
                j = 0;
                ++i;
            }
            cij[i][j] = w;
            cij[j][i] = w;
            ++j;
        }
        ++c;
    }

    return cij;
}

/**
 * @brief.
*/
std::vector<double> readDemands(std::ifstream& file, const int nbVertices)
{
    std::vector<double> demands;

    int i = 0;
    demands.reserve(nbVertices);
    while (i < nbVertices)
    {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        double di;
        iss >> di >> di; // skip index
        demands.push_back(di);
        ++i;
    }

    return demands;
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

Instance::Instance(const std::string& file_path, const int K) :
    mPath(file_path),
    mK(K)
{
    CHECK_F(std::filesystem::exists(file_path), "invalid instance path");
    init();
}


std::string Instance::getName() const
{
    return std::filesystem::path(mPath).stem().string();
}


int Instance::getNbVertices() const
{
    return mNbVertices;
}


int Instance::getK() const
{
    return mK;
}


int Instance::getC() const
{
    return mC;
}


double Instance::getcij(const int i, const int j) const
{
    DCHECK_F(i < static_cast<int>(mcij.size()), "getcij: i >= mcij.size");
    DCHECK_F(j < static_cast<int>(mcij[i].size()), "getcij: j >= mcij[j].size");
    return mcij[i][j];
}


double Instance::getdi(const int i) const
{
    DCHECK_F(i < static_cast<int>(mdi.size()), "getdi: i >= mdi.size");
    return mdi[i];
}


void Instance::show() const
{
    // TODO
}

/*------------------------------ private methods -----------------------------*/

void Instance::init()
{
    std::ifstream file(mPath);
    std::string line;
    std::string edgeWeightType;

    /* (x, y) coordinates of the vertices */
    std::vector<std::pair<double, double>> coord;

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string tmp;
        iss >> tmp;

        if (tmp == "TYPE")
        {
            iss >> tmp >> tmp;
            CHECK_F(tmp == "CVRP", "| %s", tmp.c_str());
        }
        else if (tmp == "DIMENSION")
        {
            iss >> tmp >> tmp;
            mNbVertices = stoi(tmp);
        }
        else if (tmp == "CAPACITY")
        {
            iss >> tmp >> tmp;
            mC = stoi(tmp);
        }
        else if (tmp == "EDGE_WEIGHT_TYPE")
        {
            iss >> edgeWeightType >> edgeWeightType;
        }
        else if (tmp == "NODE_COORD_SECTION")
        {
            coord = readCoordinates(file, mNbVertices);
        }
        else if (tmp == "EDGE_WEIGHT_SECTION")
        {
            mcij = readCostMtx(file, mNbVertices);
        }
        else if (tmp == "DEMAND_SECTION")
        {
            mdi = readDemands(file, mNbVertices);
        }
    }

    if (edgeWeightType == "EUC_2D")
    {
        mcij = computeDistances(coord);
    }
}