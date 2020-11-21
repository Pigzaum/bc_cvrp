////////////////////////////////////////////////////////////////////////////////
/*
 * File: init_grb_model.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Initialization VRP model helper functions declarations.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 19, 2020, 07:22 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INIT_MODEL_HPP
#define INIT_MODEL_HPP

#include <vector>
#include <memory>

#include "gurobi_c++.h"

class Instance;

namespace init
{

/**
 * @brief Variable y_ik takes value 1 if customer i is served by vehicle k and
 * 0 otherwise.
 * @param: GRBModel&:.
 * @param: std::vector<std::vector<GRBVar>>&:.
 * @param: const std::shared_ptr<const Instance>&:.
*/
void variablesY(GRBModel& model,
                std::vector<std::vector<GRBVar>>& y,
                const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Variable x_ijk takes 1 if edge (i,j) is traversed by vehicle k and 0
 * otherwise.
 * @param:.
 * @param:.
 * @param:.
*/
void variablesX(GRBModel& model,
                std::vector<std::vector<std::vector<GRBVar>>>& x,
                const std::shared_ptr<const Instance>& pInst);

/**
 * @brief Variable u_ik is the load of the vehicle k after visiting customer i.
 * This continuous variable is used when the MTZ constraints are used.
 * @param:.
 * @param:.
 * @param:.
*/
void variablesU(GRBModel& model,
                std::vector<std::vector<GRBVar>>& u,
                const std::shared_ptr<const Instance>& pInst);


/**
 * @brief .
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void singleVisitationConstrs(GRBModel& model,
                             std::vector<std::vector<GRBVar>>& y,
                             std::vector<GRBConstr>& constrs,
                             const std::shared_ptr<const Instance>& pInst);

/**
 * @brief.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void kVehiclesLeaveDepotConstr(GRBModel& model,
                               std::vector<std::vector<GRBVar>>& y,
                               std::vector<GRBConstr>& constrs,
                               const std::shared_ptr<const Instance>& pInst);

/**
 * @brief.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void degreeConstrs(GRBModel& model,
                   std::vector<std::vector<GRBVar>>& y,
                   std::vector<std::vector<std::vector<GRBVar>>>& x,
                   std::vector<GRBConstr>& constrs,
                   const std::shared_ptr<const Instance>& pInst);

/**
 * @brief.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void vehicleCapacityConstrs(GRBModel& model,
                            std::vector<std::vector<GRBVar>>& y,
                            std::vector<GRBConstr>& constrs,
                            const std::shared_ptr<const Instance>& pInst);


/**
 * @brief.
 * @param:.
 * @param:.
 * @param:.
 * @param:.
*/
void routeConnectivityConstrs(GRBModel& model,
                              std::vector<std::vector<GRBVar>>& y,
                              std::vector<std::vector<std::vector<GRBVar>>>& x,
                              std::vector<GRBConstr>& constrs,
                              const std::shared_ptr<const Instance>& pInst);

} // namespace init

#endif // INIT_MODEL_HPP