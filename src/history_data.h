#ifndef HISTORY_DATA_H
#define HISTORY_DATA_H

#include <vector>
#include <mutex>

class HistoryData {
public:
    HistoryData(std::size_t capacity);
    ~HistoryData();
    
    // Add a sample to the history
    void addSample(double value);
    
    // Get all samples
    std::vector<double> getSamples() const;
    
    // Get the average value of all samples
    double getAverage() const;
    
    // Get the maximum value
    double getMaximum() const;
    
    // Get the minimum value
    double getMinimum() const;
    
    // Get the capacity
    std::size_t getCapacity() const;
    
    // Get the current size
    std::size_t getSize() const;
    
    // Clear all samples
    void clear();

private:
    std::vector<double> m_samples;
    std::size_t m_capacity;
    mutable std::mutex m_mutex;
};

#endif // HISTORY_DATA_H
