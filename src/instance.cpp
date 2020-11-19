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

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

Instance::Instance(const std::string& file_path) :
    mPath(file_path)
{
    CHECK_F(std::filesystem::exists(file_path), "invalid instance path");
    init();
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

/*------------------------------ private methods -----------------------------*/

void Instance::init()
{
    std::ifstream file(mPath);
    std::string line;

    while (std::getline(file, line))
    {
        auto tmp = line.substr(0, line.find_first_of(" ")); // label

        if (tmp == "TYPE")
        {
            tmp = line.substr(line.find_last_of(" ") + 1);
            CHECK_F(tmp == "CVRP");
        }
        else if (tmp == "DIMENSION")
        {
            tmp = line.substr(line.find_last_of(" ") + 1);
            mNbVertices = stoi(tmp);
        }
        else if (tmp == "CAPACITY")
        {
            tmp = line.substr(line.find_last_of(" ") + 1);
            mC = stoi(tmp);
        }
        else if (tmp == "NODE_COORD_SECTION")
        {
            int i = 0;
            mCoord.reserve(mNbVertices);
            while (i < mNbVertices)
            {
                std::getline(file, line);
                std::istringstream iss(line);
                double x, y;
                iss >> x >> x >> y; // skip index
                mCoord.push_back(std::make_pair(x, y));
                ++i;
            }
        }
        else if (tmp == "DEMAND_SECTION")
        {
            int i = 0;
            mdi.reserve(mNbVertices);
            while (i < mNbVertices)
            {
                std::getline(file, line);
                std::istringstream iss(line);
                double di;
                iss >> di >> di; // skip index
                mdi.push_back(di);
                ++i;
            }
        }
    }

    mcij = computeDistances(mCoord);
}