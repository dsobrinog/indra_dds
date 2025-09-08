#pragma once

#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <cmath>

class Distribution
{
public:
    Distribution() = default;

    // Add a value to the distribution
    void addValue(double value)
    {
        _values.push_back(value);
    }

    // Number of values
    size_t size() const
    {
        return _values.size();
    }

    // Calculate mean
    double mean() const
    {
        if (_values.empty()) return 0.0;
        double sum = std::accumulate(_values.begin(), _values.end(), 0.0);
        return sum / _values.size();
    }

    // Minimum value
    double min() const
    {
        if (_values.empty()) return 0.0;
        return *std::min_element(_values.begin(), _values.end());
    }

    // Maximum value
    double max() const
    {
        if (_values.empty()) return 0.0;
        return *std::max_element(_values.begin(), _values.end());
    }

    // Mode (most frequent value)
    double mode() const
    {
        if (_values.empty()) return 0.0;

        std::unordered_map<double, int> freq;
        for (size_t i = 0; i < _values.size(); ++i)
            freq[_values[i]]++;

        int maxCount = 0;
        double modeValue = _values[0];

        for (std::unordered_map<double,int>::const_iterator it = freq.begin(); it != freq.end(); ++it)
        {
            if (it->second > maxCount)
            {
                maxCount = it->second;
                modeValue = it->first;
            }
        }

        return modeValue;
    }

    // Standard deviation
    double stdDev() const
    {
        if (_values.size() < 2) return 0.0;
        double m = mean();
        double accum = 0.0;
        for (size_t i = 0; i < _values.size(); ++i)
        {
            double diff = _values[i] - m;
            accum += diff * diff;
        }
        return std::sqrt(accum / (_values.size() - 1));
    }

    // Print all statistics
    void printReport() const
    {
        std::cout << "\n**** DISTRIBUTION REPORT ****\n";
        std::cout << "Count: " << _values.size() << "\n";
        std::cout << "Mean: " << mean() << "\n";
        std::cout << "Min: " << min() << "\n";
        std::cout << "Max: " << max() << "\n";
        std::cout << "Mode: " << mode() << "\n";
        std::cout << "StdDev: " << stdDev() << "\n";
        std::cout << "-----------------------------\n";
    }

private:
    std::vector<double> _values;
};
