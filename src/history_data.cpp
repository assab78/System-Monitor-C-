#include "history_data.h"
#include <algorithm>
#include <numeric>

HistoryData::HistoryData(std::size_t capacity) : m_capacity(capacity) {
    m_samples.reserve(capacity);
}

HistoryData::~HistoryData() {
    // Default destructor
}

void HistoryData::addSample(double value) {
    std::lock_guard<std::mutex> lock(m_mutex);
    
    // If we've reached capacity, remove the oldest sample
    if (m_samples.size() >= m_capacity) {
        m_samples.erase(m_samples.begin());
    }
    
    m_samples.push_back(value);
}

std::vector<double> HistoryData::getSamples() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples;
}

double HistoryData::getAverage() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_samples.empty()) {
        return 0.0;
    }
    
    double sum = std::accumulate(m_samples.begin(), m_samples.end(), 0.0);
    return sum / m_samples.size();
}

double HistoryData::getMaximum() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_samples.empty()) {
        return 0.0;
    }
    
    return *std::max_element(m_samples.begin(), m_samples.end());
}

double HistoryData::getMinimum() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_samples.empty()) {
        return 0.0;
    }
    
    return *std::min_element(m_samples.begin(), m_samples.end());
}

std::size_t HistoryData::getCapacity() const {
    return m_capacity;
}

std::size_t HistoryData::getSize() const {
    std::lock_guard<std::mutex> lock(m_mutex);
    return m_samples.size();
}

void HistoryData::clear() {
    std::lock_guard<std::mutex> lock(m_mutex);
    m_samples.clear();
}
