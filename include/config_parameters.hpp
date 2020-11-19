////////////////////////////////////////////////////////////////////////////////
/*
 * File: config_parameters.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief Configuration input parameters class declaration.
 * (see ./input/example.cfg).
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on January 9, 2020, 11:58 PM
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef CONFIG_PARAMETERS_HPP
#define CONFIG_PARAMETERS_HPP

#include <map>
#include <string>

class ConfigParameters
{
public:

    /**
     * @brief Solver parameters.
    */
    struct solver
    {
        bool show_log;          // print output parameters
        std::size_t time_limit;  // time limit of the solver
        std::size_t nb_threads; // # of threads used by solver
    };

    struct model
    {
        int K_;
    };

    /**
     * @brief Default constructor, copy constructor, move constructor,
     * destructor, copy assingment operator and move assingment operator.
    */
    ConfigParameters() = default;
    ConfigParameters(const ConfigParameters& other) = default;
    ConfigParameters(ConfigParameters&& other) = default;
    ~ConfigParameters() = default;
    ConfigParameters& operator=(const ConfigParameters& other) = default;
    ConfigParameters& operator=(ConfigParameters&& other) = default;

    /**
     * @brief Constructor from file.
     * @param const std::string&: configuration file path with values of all
     * parameters.
    */
    ConfigParameters(const std::string config_path);

    /**
     * @brief Get the instance path.
     * @return {std::string}:.
    */
    std::string getInstancePath() const;

    /**
     * @brief Get the default output folder path.
     * @return {std::string}:.
    */
    std::string getOutputDir() const;

    /**
     * @brief.
    */
    model getModelParams() const;

    /**
     * @brief.
    */
    solver getSolverParams() const;

    /**
     * @brief Prints instance value.
    */
    void show() const;

private:

    // Instance path (or instaces directory path)
    std::string mInstancePath;

    // Output folder path.
    std::string mOutputDir;

    // Model parameters
    ConfigParameters::model mModelParam;

    // Solver parameters
    ConfigParameters::solver mSolverParam;

    // Config_parameters values (from input file) mapping.
    std::map<std::string, std::string> mData;

    /**
     * @brief Initializes the parameters.
    */
    void setupParameters();
};

#endif // CONFIG_PARAMETERS_HPPs