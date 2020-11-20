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
 * @brief.
 * @param:.
 * @param:.
 * @param:.
*/
void variablesY(GRBModel& model,
                std::vector<std::vector<GRBVar>>& y,
                const std::shared_ptr<const Instance>& pInst);

/**
 * @brief.
 * @param:.
 * @param:.
 * @param:.
*/
void variablesX(GRBModel& model,
                std::vector<std::vector<std::vector<GRBVar>>>& x,
                const std::shared_ptr<const Instance>& pInst);

} // namespace init

#endif // INIT_MODEL_HPP