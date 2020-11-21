////////////////////////////////////////////////////////////////////////////////
/*
 * File: vrp_lp.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief CVRP class declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 1, 2020, 02:11 PM.
 * 
 * References:
 * [1].
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef VRP_LP_HPP
#define VRP_LP_HPP

#include <memory>

#include "gurobi_c++.h"

#include "config_parameters.hpp"
#include "callback/callback_sec.hpp"

class Instance;

class VrpLp
{
public:

    /**
     * @brief Default constructor, copy constructor, move constructor,
     * destructor, copy assingment operator and move assingment operator.
    */
    VrpLp() = default;
    VrpLp(const VrpLp& other) = default;
    VrpLp(VrpLp&& other) = default;
    ~VrpLp() = default;
    VrpLp& operator=(const VrpLp& other) = default;
    VrpLp& operator=(VrpLp&& other) = default;

    /**
     * @brief Constructs from an instance.
     * @param: const std::shared_ptr<const Instance>&: pointer to an instance.
    */
    VrpLp(const std::shared_ptr<const Instance>& pInst,
          const ConfigParameters::model& params);

    bool solve(const ConfigParameters::solver& params);

    void writeIis(std::string path);
    void writeModel(std::string path);
    void writeResultsJSON(std::string path);
    void writeSolution(std::string path);

private:

    // pointer to instance
    std::shared_ptr<const Instance> mpInst;

    GRBEnv mEnv;
    GRBModel mModel;
    std::vector<std::vector<GRBVar>> m_y;
    std::vector<std::vector<GRBVar>> m_u; // used by MTZ constraints
    std::vector<std::vector<std::vector<GRBVar>>> m_x;
    std::vector<GRBConstr> mConstrs;

    CallbackSEC mCbSEC;
};

#endif // VRP_LP_HPP
