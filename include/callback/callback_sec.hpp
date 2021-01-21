////////////////////////////////////////////////////////////////////////////////
/*
 * File: callback_sec.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Callback class declaration for lazy/cut subtour separation
 * constraints.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 15, 2020, 11:59 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef CALLBACK_SEC_HPP
#define CALLBACK_SEC_HPP

#include <memory>

#include "gurobi_c++.h"

#include "../instance.hpp"

class CallbackSEC : public GRBCallback
{
public:

    enum class constrsType {lazy, cut};

    CallbackSEC(const CallbackSEC& other) = default;
    CallbackSEC(CallbackSEC&& other) = default;
    ~CallbackSEC() = default;

    CallbackSEC() = delete;
    CallbackSEC& operator=(const CallbackSEC& other) = delete;
    CallbackSEC& operator=(CallbackSEC&& other) = delete;

    CallbackSEC(const std::vector<std::vector<std::vector<GRBVar>>>& x,
                const std::vector<std::vector<GRBVar>>& y,
                const std::shared_ptr<const Instance>& p_inst);

private:

    // equal to one if j immediately follows i in the route traveled at time t
    const std::vector<std::vector<std::vector<GRBVar>>>& m_x;
    // retailer i is served at time t
    const std::vector<std::vector<GRBVar>>& m_y;

    std::shared_ptr<const Instance> mpInst;

    void callback() override;

    int addCVRPSEPCAP(const constrsType cstType);

    /**
     * @brief Retrieve the x variables values from the relaxation solution at
     * the current node.
     * @param: constraint type.
     * @return: x variables values at the current node.
    */
    std::vector<std::vector<std::vector<double>>> getxVarsValues(
        const constrsType cstType);

    /**
     * @brief Retrieve the y variables values from the relaxation solution at
     * the current node.
     * @param: constraint type.
     * @return: y variables values at the current node.
    */
    std::vector<std::vector<double>> getyVarsValues(const constrsType cstType);
};

#endif // CALLBACK_SEC_HPP