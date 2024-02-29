#include <iostream>
#include <iomanip>
#include <list>
#include <unordered_map>
#include <cmath>
#include <iterator>
#include <algorithm>
#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <sstream>

class MutableInt {

public:
    int value;

    MutableInt() : value(0) {}

    MutableInt(int val) : value(val) {}

    void set(int val) {
        value = val;
    }

    int get() const {
        return value;
    }
};

class Bench {
    
private:
    static const int DEFAULT_WARMUP = 0;
    static const int NUMBER_OF_DECIMALS = 3;
    static const bool INCLUDE_STDEV = true;
    static const bool INCLUDE_MORE_PERCS = false;
    static const bool INCLUDE_WORST_PERCS = false;
    static const bool INCLUDE_TOTALS = false;

    std::chrono::high_resolution_clock::time_point time;
    long long count;
    long long totalTime;
    int warmup;
    long long minTime;
    long long maxTime;

    int size;
    std::list<MutableInt> pool;
    std::unordered_map<long long, MutableInt> results;
    std::vector<long long> tempList;

public:
    Bench() : Bench(DEFAULT_WARMUP) {}

    Bench(const int warmup) : warmup(warmup) {
        reset(true);
    }

    void reset(bool repeatWarmup) {
        time = std::chrono::high_resolution_clock::now();
        count = 0;
        totalTime = 0;
        if (!repeatWarmup) warmup = 0;
        minTime = LLONG_MAX;
        maxTime = LLONG_MIN;

        size = 0;
        results.clear();
        pool.clear();
    }

    std::chrono::high_resolution_clock::time_point mark() {
        time = std::chrono::high_resolution_clock::now();
        return time;
    }

    long long measure() {
        if (time.time_since_epoch().count() > 0) {
            auto now = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(now - time).count();
            if (elapsed < 0) throw std::runtime_error("Found a negative elapsed time");
            const bool counted = measure(elapsed);
            if (counted) {
                return elapsed;
            }
        }
        return -1;
    }

    bool isWarmingUp() const {
        return warmup <= count;
    }

    long long getCount() const {
        return count;
    }

    double getAverage() const {
        return avg();
    }

    void printResults() {
        std::cout << resultsStr() << std::endl << std::endl;
    }

    static void doSleep(Bench& bench) {
        bench.mark();
        std::this_thread::sleep_for(std::chrono::nanoseconds(1000));
        bench.measure();
    }

private:
    MutableInt& getMutableInt(int x) {
        if (!pool.empty()) {
            MutableInt& toReturn = pool.back();
            pool.pop_back();
            toReturn.set(x);
            return toReturn;
        } else {
            pool.emplace_back(x);
            return pool.back();
        }
    }

    void releaseMutableInt(MutableInt& mi) {
        pool.push_back(mi);
    }

    bool measure(long long lastNanoTime) {
        if (++count > warmup) {
            totalTime += lastNanoTime;
            minTime = std::min(minTime, lastNanoTime);
            maxTime = std::max(maxTime, lastNanoTime);

            auto countIt = results.find(lastNanoTime);
            if (countIt == results.end()) {
                results[lastNanoTime] = getMutableInt(1);
            } else {
                countIt->second.set(countIt->second.get() + 1);
            }
            size++;
        }
        return false;
    }

    double avg() const {
        const long long realCount = count - warmup;
        if (realCount <= 0) {
            return 0;
        }
        const double avg = static_cast<double>(totalTime) / static_cast<double>(realCount);
        const double rounded = round(avg, NUMBER_OF_DECIMALS);
        return rounded;
    }

    double round(double d, int decimals) const {
        double pow = std::pow(10, decimals);
        return std::round(d * pow) / pow;
    }

    double round(double d) const {
        return round(d, NUMBER_OF_DECIMALS);
    }

    std::string convertNanoTime(double nanoTime) const {
        std::string result;
        return convertNanoTime(nanoTime, result);
    }

    std::string& convertNanoTime(double nanoTime, std::string& result) const {
        if (nanoTime >= 1000000000L) {
            double seconds = round(nanoTime / 1000000000.0);
            result += std::to_string(seconds) + (seconds > 1 ? " secs" : " sec");
        } else if (nanoTime >= 1000000L) {
            double millis = round(nanoTime / 1000000.0);
            result += std::to_string(millis) + (millis > 1 ? " millis" : " milli");
        } else if (nanoTime >= 1000L) {
            double micros = round(nanoTime / 1000.0);
            result += std::to_string(micros) + (micros > 1 ? " micros" : " micro");
        } else {
            double nanos = round(nanoTime);
            result += std::to_string(nanos) + (nanos > 1 ? " nanos" : " nano");
        }
        return result;
    }

    std::string formatter(int value) const {
        std::stringstream ss;
        ss.imbue(std::locale(""));
        ss << std::fixed << value;
        return ss.str();
    }

    void addPercentile(std::string& result, double perc, std::map<long long, MutableInt>& treeMap) {
        
        if (treeMap.empty()) {
            return;
        }

        tempList.clear();
        double stdevTop = -1;

        long long maxTop = -1;
        long long minBottom = -1;

        long long x = std::round(perc * size);
        auto iter = treeMap.begin();
        int iTop = 0;
        int iBottom = 0;
        long long sumTop = 0;
        long long sumBottom = 0;
        bool trueForTopFalseForBottom = true;
        while (iter != treeMap.end()) {
            const long long time = iter->first;
            const MutableInt& count = iter->second;
            for (int a = 0; a < count.get(); a++) {
                if (trueForTopFalseForBottom) {
                    iTop++;
                    sumTop += time;
                    addTempTime(time);
                    if (iTop == x) {
                        maxTop = time;

                        if (INCLUDE_STDEV) {
                            double avg = static_cast<double>(sumTop) / static_cast<double>(iTop);
                            long long sum = 0;
                            auto iter2 = tempList.begin();
                            while (iter2 != tempList.end()) {
                                long long t = *iter2;
                                sum += (avg - t) * (avg - t);
                                ++iter2;
                            }
                            double stdev = std::sqrt(static_cast<double>(sum) / static_cast<double>(tempList.size()));
                            double rounded = round(stdev, 2);
                            stdevTop = rounded;
                        }

                        if (INCLUDE_WORST_PERCS) {
                            trueForTopFalseForBottom = false;
                            tempList.clear();
                        } else {
                            goto END_LOOP;
                        }
                    }
                } else {
                    iBottom++;
                    sumBottom += time;
                    if (minBottom == -1) {
                        minBottom = time;
                    }
                    addTempTime(time);
                }
            }
            ++iter;
        }
        END_LOOP:

        result += " | " + formatPercentage(perc, 8);
        if (INCLUDE_TOTALS) result += " (" + formatter(iTop) + ")";
        result += " = [avg: " + convertNanoTime(sumTop / iTop);
        if (INCLUDE_STDEV) {
            result += ", stdev: " + std::to_string(stdevTop) + " nanos";
        }
        result += ", max: " + convertNanoTime(maxTop) + ']';
        if (INCLUDE_WORST_PERCS) {
            result += " - " + formatPercentage(1 - perc, 8);
            if (INCLUDE_TOTALS) result += " (" + (iBottom > 0 ? formatter(iBottom) : "0") + ")";
            result += " = [avg: " + (iBottom > 0 ? convertNanoTime(sumBottom / iBottom) : "?");
            if (INCLUDE_STDEV) {
                result += ", stdev: ";
                if (iBottom > 0) {
                    double avg = static_cast<double>(sumBottom) / static_cast<double>(iBottom);
                    long long sum = 0;
                    auto iter2 = tempList.begin();
                    while (iter2 != tempList.end()) {
                        long long t = *iter2;
                        sum += (avg - t) * (avg - t);
                        ++iter2;
                    }
                    double stdevBottom = std::sqrt(static_cast<double>(sum) / static_cast<double>(tempList.size()));
                    double rounded = round(stdevBottom, 2);
                    result += std::to_string(rounded) + " nanos";
                } else {
                    result += "?";
                }
            }
            result += ", min: " + (minBottom != -1 ? convertNanoTime(minBottom) : "?") + ']';
        }
    }

    std::string resultsStr() {
        std::string result;
        const long long realCount = count - warmup;
        result += "Iterations: " + formatter(realCount) + " | Warm-Up: " + formatter(warmup) + " | Avg Time: " +
                  convertNanoTime(avg());
        if (realCount > 0) {
            result += " | Min Time: " + convertNanoTime(minTime) + " | Max Time: " + convertNanoTime(maxTime);
        }

        std::map<long long, MutableInt> treeMap(results.begin(), results.end());

        addPercentile(result, 0.75, treeMap);
        addPercentile(result, 0.9, treeMap);
        addPercentile(result, 0.99, treeMap);
        addPercentile(result, 0.999, treeMap);
        addPercentile(result, 0.9999, treeMap);
        addPercentile(result, 0.99999, treeMap);
        if (INCLUDE_MORE_PERCS) {
            addPercentile(result, 0.999999, treeMap);
            addPercentile(result, 0.9999999, treeMap);
        }

        return result;
    }

    std::string formatPercentage(double x, int decimals) const {
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(decimals) << x;
        return ss.str();
    }

    void addTempTime(long long time) {
        tempList.push_back(time);
    }
};

int main() {
    const int warmupIterations = 1000000;
    const int measurementIterations = 1000000;

    Bench bench(warmupIterations);

    while (bench.getCount() < measurementIterations + warmupIterations) {
        Bench::doSleep(bench);
    }

    bench.printResults();

    bench.reset(false); // false because we don't want to repeat warmup

    while (bench.getCount() < measurementIterations) {
        Bench::doSleep(bench);
    }

    bench.printResults();

    return 0;
}

