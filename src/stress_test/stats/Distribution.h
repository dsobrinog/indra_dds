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

    std::string name_distribution = "DISTRIBUTION";

    inline void set_distribution_name(std::string name)
    {
        name_distribution = name;
    }

    void addValue(double value)
    {
        _values.push_back(value);
    }

    size_t size() const
    {
        return _values.size();
    }

    double mean() const
    {
        if (_values.empty()) return 0.0;
        double sum = std::accumulate(_values.begin(), _values.end(), 0.0);
        return sum / _values.size();
    }

    double min() const
    {
        if (_values.empty()) return 0.0;
        return *std::min_element(_values.begin(), _values.end());
    }

    double max() const
    {
        if (_values.empty()) return 0.0;
        return *std::max_element(_values.begin(), _values.end());
    }

    double mode() const
    {
        if (_values.empty()) return 0.0;

        std::unordered_map<double, int> freq;
        for (size_t i = 0; i < _values.size(); ++i)
            freq[_values[i]]++;

        int maxCount = 0;
        double modeValue = _values[0];

        for (auto it = freq.begin(); it != freq.end(); ++it)
        {
            if (it->second > maxCount)
            {
                maxCount = it->second;
                modeValue = it->first;
            }
        }

        return modeValue;
    }

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

    // --- Local jitter ---
    double localJitter() const
    {
        if (_values.size() < 2) return 0.0;

        // 1. Compute deltas between consecutive samples
        std::vector<double> diffs;
        diffs.reserve(_values.size() - 1);

        for (size_t i = 1; i < _values.size(); ++i)
        {
            diffs.push_back(std::abs(_values[i] - _values[i - 1]));
        }

        // 2. Mean of deltas
        double meanDiff = std::accumulate(diffs.begin(), diffs.end(), 0.0) / diffs.size();

        // 3. Standard deviation of deltas
        double accum = 0.0;
        for (size_t i = 0; i < diffs.size(); ++i)
        {
            double diff = diffs[i] - meanDiff;
            accum += diff * diff;
        }

        return std::sqrt(accum / (diffs.size() - 1));
    }

    void printReport() const
    {
        std::cout << "\n**** " << name_distribution <<" REPORT ****\n";
        std::cout << "Count: " << _values.size() << "\n";
        std::cout << "Mean: " << mean() << "\n";
        std::cout << "Min: " << min() << "\n";
        std::cout << "Max: " << max() << "\n";
        std::cout << "Mode: " << mode() << "\n";
        std::cout << "StdDev (global): " << stdDev() << "\n";
        std::cout << "Local Jitter: " << localJitter() << "\n";
        std::cout << "-----------------------------\n";
    }

private:
    std::vector<double> _values;
};
