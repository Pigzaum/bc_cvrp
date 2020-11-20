////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.cpp
 * Author: Guilherme O. Chagas
 *
 * @brief Initialization VRP model helper functions definitions.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 19, 2020, 07:28 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/init_grb_model.hpp"
#include "../include/instance.hpp"


void init::variablesY(GRBModel& model,
                      std::vector<std::vector<GRBVar>>& y,
                      const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "Initializing variables y...");

    y.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        y.push_back(std::vector<GRBVar>());
        y[i].reserve(pInst->getK());
        for (int k = 0; k < pInst->getK(); ++k)
        {
            std::ostringstream oss;
            oss << "y_" << i << "_" << k;
            y[i].push_back(model.addVar(0, 1, 0, GRB_BINARY, oss.str()));
        }
    }
}


void init::variablesX(GRBModel& model,
                      std::vector<std::vector<std::vector<GRBVar>>>& x,
                      const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "Initializing variables x...");

    x.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        x.push_back(std::vector<std::vector<GRBVar>>());
        x[i].reserve(pInst->getNbVertices());
        for (int j = 0; j < pInst->getNbVertices(); ++j)
        {
            x[i].push_back(std::vector<GRBVar>(pInst->getK()));

            if (j <= i) continue;

            for (int k = 0; k < pInst->getK(); ++k)
            {
                std::ostringstream oss;
                oss << "x_" << i << "_" << j << "_" << k;
                x[i][j][k] = model.addVar(0,
                                          (i == 0 ? 2 : 1),
                                          pInst->getcij(i, j),
                                          GRB_BINARY,
                                          oss.str());
            }
        }
    }
}