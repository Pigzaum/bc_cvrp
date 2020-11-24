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

#include <bitset>
#include <cmath>
#include <sstream>

#include "../include/ext/loguru/loguru.hpp"

#include "../include/init_grb_model.hpp"
#include "../include/instance.hpp"

/////////////////////////////// Helper functions ///////////////////////////////

namespace
{

std::pair<std::vector<int>, std::vector<int>> findCutS(const int idx,
                                                       const int nbVertices)
{
    const int MAX_N = 30;
    CHECK_F(nbVertices < MAX_N, "You shouldn't use SEC with |V| > 30");

    std::vector<int> inS;
    std::vector<int> notInS;
    inS.reserve(nbVertices);
    notInS.reserve(nbVertices);

    std::bitset<MAX_N> myBitset(idx);
    for (int i = 1; i < nbVertices; ++i)
    {
        if (myBitset[i - 1])
        {
            inS.push_back(i);
        }
        else
        {
            notInS.push_back(i);
        }
    }

    notInS.push_back(0);

    return std::make_pair(inS, notInS);
}

} // anonymous namespace

////////////////////////////////////////////////////////////////////////////////

void init::variablesY(GRBModel& model,
                      std::vector<std::vector<GRBVar>>& y,
                      const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing variables y...");

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
    RAW_LOG_F(INFO, "\tInitializing variables x...");

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
                                          GRB_INTEGER,
                                          oss.str());
            }
        }
    }
}


void init::variablesU(GRBModel& model,
                      std::vector<std::vector<GRBVar>>& u,
                      const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing variables u...");

    u.reserve(pInst->getNbVertices());
    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        u.push_back(std::vector<GRBVar>(pInst->getK()));

        if (i == 0) continue;

        for (int k = 0; k < pInst->getK(); ++k)
        {
            std::ostringstream oss;
            oss << "u_" << i << "_" << k;
            u[i][k] = model.addVar(pInst->getdi(i),
                                   pInst->getC(),
                                   0,
                                   GRB_CONTINUOUS,
                                   oss.str());
        }
    }
}


void init::singleVisitationConstrs(GRBModel& model,
                                   std::vector<GRBConstr>& constrs,
                                   const std::vector<std::vector<GRBVar>>& y,
                                   const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing single visitation constraints...");

    for (int i = 1; i < pInst->getNbVertices(); ++i)
    {
        GRBLinExpr e = 0;
        for (int k = 0; k < pInst->getK(); ++k)
        {
            e += y[i][k];
        }
        std::ostringstream oss;
        oss << "1C_" << i;
        constrs.push_back(model.addConstr(e == 1, oss.str()));
    }
}


void init::kVehiclesLeaveDepotConstr(
    GRBModel& model,
    std::vector<GRBConstr>& constrs,
    const std::vector<std::vector<GRBVar>>& y,
    const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing K vehicles leave depot constraints...");

    GRBLinExpr e = 0;
    for (int k = 0; k < pInst->getK(); ++k)
    {
        e += y[0][k];
    }

    std::ostringstream oss;
    oss << "2C";
    constrs.push_back(model.addConstr(e == pInst->getK(), oss.str()));
}


void init::degreeConstrs(
    GRBModel& model,
    std::vector<GRBConstr>& constrs,
    const std::vector<std::vector<GRBVar>>& y,
    const std::vector<std::vector<std::vector<GRBVar>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing degree constraints...");

    for (int i = 0; i < pInst->getNbVertices(); ++i)
    {
        for (int k = 0; k < pInst->getK(); ++k)
        {
            GRBLinExpr e = 0;
            for (int j = 0; j < pInst->getNbVertices(); ++j)
            {
                if (i < j)
                {
                    e += x[i][j][k];
                }
                else if (i > j)
                {
                    e += x[j][i][k];
                }
            }

            std::ostringstream oss;
            oss << "3C_" << i << "_" << k;
            constrs.push_back(model.addConstr(e == 2 * y[i][k], oss.str()));
        }
    }
}


void init::vehicleCapacityConstrs(GRBModel& model,
                                  std::vector<GRBConstr>& constrs,
                                  const std::vector<std::vector<GRBVar>>& y,
                                  const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing vehicle capacity constraints...");

    for (int k = 0; k < pInst->getK(); ++k)
    {
        GRBLinExpr e = 0;
        for (int i = 0; i < pInst->getNbVertices(); ++i)
        {
            e += pInst->getdi(i) * y[i][k];
        }

        std::ostringstream oss;
        oss << "4C_" << k;
        constrs.push_back(model.addConstr(e <= pInst->getC(), oss.str()));
    }
}


void init::routeConnectivityConstrs(
    GRBModel& model,
    std::vector<GRBConstr>& constrs,
    const std::vector<std::vector<GRBVar>>& y,
    const std::vector<std::vector<std::vector<GRBVar>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing route connectivity constraints...");

    const auto nbSets = std::pow(2, pInst->getNbVertices() - 1);
    for (int c = 1; c < nbSets; ++c)
    {
        auto [inS, notInS] = findCutS(c, pInst->getNbVertices());
        for (int k = 0; k < pInst->getK(); ++k)
        {
            for (auto h : inS)
            {
                GRBLinExpr e = 0;
                for (auto i : inS)
                {
                    for (auto j : notInS)
                    {
                        if (i < j)
                        {
                            e += x[i][j][k];
                        }
                        else
                        {
                            e += x[j][i][k];
                        }
                    }
                }

                std::ostringstream oss;
                oss << "5CA_" << c << "_" << h << "_" << k;
                constrs.push_back(model.addConstr(e >= 2 * y[h][k], oss.str()));
            }
        }
    }
}


void init::subtourEliminationConstrs(
    GRBModel& model,
    std::vector<GRBConstr>& constrs,
    const std::vector<std::vector<std::vector<GRBVar>>>& x,
    const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing SEC constraints...");

    const auto nbSets = std::pow(2, pInst->getNbVertices() - 1);
    for (int c = 1; c < nbSets; ++c)
    {
        auto S = findCutS(c, pInst->getNbVertices()).first;
        if (S.size() > 1)
        {
            for (int k = 0; k < pInst->getK(); ++k)
            {
                GRBLinExpr lhs = 0;
                for (auto i : S)
                {
                    for (auto j : S)
                    {
                        if (i < j)
                        {
                            lhs += x[i][j][k];
                        }
                    }
                }

                GRBLinExpr rhs = S.size() - 1;

                std::ostringstream oss;
                oss << "5CB_" << c << "_" << k;
                constrs.push_back(model.addConstr(lhs <= rhs, oss.str()));
            }
        }
    }
}


void init::MTZConstrs(GRBModel& model,
                      std::vector<GRBConstr>& constrs,
                      const std::vector<std::vector<GRBVar>>& u,
                      const std::vector<std::vector<std::vector<GRBVar>>>& x,
                      const std::shared_ptr<const Instance>& pInst)
{
    RAW_LOG_F(INFO, "\tInitializing MTZ constraints...");

    for (int i = 1; i < pInst->getNbVertices(); ++i)
    {
        for (int j = 1; j < pInst->getNbVertices(); ++j)
        {
            if (i == j) continue;

            if (pInst->getdi(i) + pInst->getdi(j) <= pInst->getC())
            {
                for (int k = 0; k < pInst->getK(); ++k)
                {
                    GRBLinExpr lhs = u[i][k] - u[j][k];
                    GRBLinExpr rhs = pInst->getC() - pInst->getdi(j);

                    if (i < j)
                    {
                        lhs += pInst->getC() * x[i][j][k];

                        std::ostringstream oss;
                        oss << "5CC_" << i << "_" << j << "_" << k;
                        constrs.push_back(model.addConstr(lhs <= rhs,
                                                          oss.str()));
                    }
                }
            }
        }
    }
}