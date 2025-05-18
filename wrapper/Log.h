#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <mutex>
#include <ctime>
#include <chrono>
#include <iomanip>
#include <sstream>

class Logger {
public:
    template<typename T>
    Logger& operator<<(const T& value) {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!timestampAdded_) {
            std::cout << getCurrentTime() << " ";
            timestampAdded_ = true;
        }
        std::cout << value;
        return *this;
    }

    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::lock_guard<std::mutex> lock(mutex_);
        std::cout << manip;
        if (manip == static_cast<std::ostream & (*)(std::ostream&)>(std::endl)) {
            timestampAdded_ = false; 
        }
        return *this;
    }

private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
        std::tm local_time = *std::localtime(&now_time);
        std::ostringstream oss;
        oss << std::put_time(&local_time, "%Y/%m/%d %H:%M:%S");
        return oss.str();
    }

    std::mutex mutex_; // ������
    bool timestampAdded_ = false; // ��־λ����ʾ�Ƿ������ʱ���
};

// ȫ��ʵ��
extern Logger logcout;

#endif