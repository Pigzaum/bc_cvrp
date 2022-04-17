////////////////////////////////////////////////////////////////////////////////
/*
 * File: instance.hpp
 * Author: Guilherme O. Chagas
 *
 * @brief CVRP instance class declaration.
 *
 * (I'm sorry for my bad english xD)
 *
 * Created on November 18, 2020, 01:27 PM.
 * 
 * References:
 */
////////////////////////////////////////////////////////////////////////////////

#ifndef INSTANCE_HPP
#define INSTANCE_HPP

#include <string>
#include <vector>

class Instance
{
public:

    /**
     * @brief Default constructor, copy constructor, move constructor,
     * destructor, copy assingment operator and move assingment operator.
    */
    Instance() = default;
    Instance(const Instance& other) = default;
    Instance(Instance&& other) = default;
    ~Instance() = default;
    Instance& operator=(const Instance& other) = default;
    Instance& operator=(Instance&& other) = default;

    /**
     * @brief Constructs from a instance file.
     * @param : const std::string&: instance file path.
     * @param: const int: number of vehicles.
    */
    Instance(const std::string& file_path, const int K);

    std::string getName() const;

    int getNbVertices() const;
    int getK() const;
    int getC() const;
    double getcij(const int i, const int j) const;
    double getdi(const int i) const;

    void show() const;

private:

    // instance full path
    std::string mPath;

    // number of vertices (depot and customers)
    int mNbVertices; 

    // number of vehicles
    int mK;

    // transportation capacity
    int mC;

    int mCk; // heterogeneous fleet

    // vertices demand
    std::vector<double> mdi;

    // distance matrix
    std::vector<std::vector<double>> mcij;

    void init();
};

#endif // INSTANCE_HPP